
// MicroJsonParser.cpp

#include "MicroJsonParser.h"
#include <Arduino.h>
#include <FS.h>

#define LOGGER_MODULE "mj"
#include "Logger.h"

// Remarks:
// * No Arrays
// * no unquoted names

// State definitions. Use some bits for special functionality

// MJ_IGNOREBLANCS marks the states where blank input characters are of no
// relevance and can be ignored.
#define MJ_IGNOREBLANCS 0x100

// The states while parsing JSON.
#define MJ_STATE_INIT (0x01 + MJ_IGNOREBLANCS)
#define MJ_STATE_PRE_NAME (0x02 + MJ_IGNOREBLANCS)
#define MJ_STATE_Q_NAME (0x03)
#define MJ_STATE_NAME (0x04)
#define MJ_STATE_ASSIGN (0x11 + MJ_IGNOREBLANCS)
#define MJ_STATE_PRE_VALUE (0x21 + MJ_IGNOREBLANCS)
#define MJ_STATE_Q_VALUE (0x22)
#define MJ_STATE_Q_VALUE_ESC (0x23)
#define MJ_STATE_Q_VALUE_ESCU (0x24)
#define MJ_STATE_VALUE (0x25)
#define MJ_STATE_PRE_DONE (0x31 + MJ_IGNOREBLANCS)
#define MJ_STATE_DONE (0x32 + MJ_IGNOREBLANCS)
#define MJ_STATE_ERROR (0x50 + MJ_IGNOREBLANCS)

#define NUL '\0'

#if 0
// DEBUG State transitions
#define MJ_NEWSTATE(newState) (LOGGER_INFO("state %x", newState), newState)
#else
#define MJ_NEWSTATE(newState) (newState)
#endif

#define MJ_ERROR(reason) (LOGGER_ERR(reason), MJ_STATE_ERROR)


MicroJson::MicroJson(MicroJsonCallbackFn callback)
{
  _callbackFn = callback;
  _state = MJ_STATE_INIT;
  _level = 0;
  _path[0] = NUL;
  _name[0] = NUL;
  _value[0] = NUL;
  _esc[0] = NUL;
};


void MicroJson::parse(const char *s)
{
  _state = MJ_STATE_INIT;
  _level = 0;
  while ((s != NULL) && (*s != '\0')) {
    parse(*s++);
  }
};


void MicroJson::parseFile(const char *fName)
{
  char buffer[64];
  char *p;
  size_t len;

  if (SPIFFS.exists(fName)) {
    LOGGER_TRACE("parsing file %s", fName);

    _state = MJ_STATE_INIT;
    _level = 0;

    File file = SPIFFS.open(fName, "r");
    while (file.available()) {
      len = file.readBytes(buffer, sizeof(buffer));
      p = buffer;
      while (len > 0) {
        parse(*p++);
        len--;
      }
    }
    file.close();
  } // if
};


// append a single character and the NUL byte.
void strncat(char *s, char ch, int len)
{
  // Search terminating NUL
  while ((len > 0) && (*s != NUL)) {
    len--;
    s++;
  }
  // append new character and string termination
  if (len > 1) {
    *s++ = ch;
    *s = NUL;
  }
} // strncat()


/**
 * @brief get the next character from the input and parse.
 */
void MicroJson::parse(char ch)
{
  // LOGGER_INFO("parse(%c)", ch);
  if ((_state && MJ_IGNOREBLANCS) && isblank(ch)) {
    // ignore white space here.
    // LOGGER_INFO(" ignored");

  } else if (_state == MJ_STATE_ERROR) {
    // stay in error status
    // LOGGER_TRACE(" -err");

  } else if (_state == MJ_STATE_DONE) {
    // already finished, anything is error
    _state = MJ_ERROR("unexpected character at end");

  } else if (_state == MJ_STATE_INIT) {
    if (ch != '{') {
      _state = MJ_ERROR("'{' expected");
    } else {
      _level++;
      _state = MJ_NEWSTATE(MJ_STATE_PRE_NAME);
      _name[0] = _value[0] = NUL;
      if (_callbackFn)
        (_callbackFn)(_level, _path, NULL, NULL);
    }

  } else if ((_state == MJ_STATE_PRE_NAME) && (ch == '"')) {
    _state = MJ_NEWSTATE(MJ_STATE_Q_NAME);

  } else if (_state == MJ_STATE_Q_NAME) {
    if (ch != '"') {
      strncat(_name, ch, sizeof(_name));
    } else {
      // LOGGER_TRACE(" name=%s", _name);
      _state = MJ_NEWSTATE(MJ_STATE_ASSIGN);
    } // if

  } else if (_state == MJ_STATE_ASSIGN) {
    if (ch == ':') {
      _state = MJ_NEWSTATE(MJ_STATE_PRE_VALUE);
    } else {
      _state = MJ_ERROR("':' expected");
    }


  } else if (_state == MJ_STATE_PRE_VALUE) {
    if (ch == '"') {
      _state = MJ_NEWSTATE(MJ_STATE_Q_VALUE);

    } else if (ch == '{') {
      _level++;
      if (_path[0] != NUL)
        strncat(_path, MICROJSON_PATH_SEPARATOR, sizeof(_path));
      strncat(_path, _name, sizeof(_path));
      _name[0] = _value[0] = NUL;
      if (_callbackFn)
        (_callbackFn)(_level, _path, NULL, NULL);
      _state = MJ_NEWSTATE(MJ_STATE_PRE_NAME);

    } else if (isdigit(ch)) {
      strncat(_value, ch, sizeof(_value));
      _state = MJ_NEWSTATE(MJ_STATE_VALUE);
    }

  } else if (_state == MJ_STATE_VALUE) {
    if (isdigit(ch)) {
      strncat(_value, ch, sizeof(_value));

    } else {
      // this character doesn't belong to the value any more.
      // LOGGER_TRACE(" value=%s", _value);
      if (_callbackFn)
        (_callbackFn)(_level, _path, _name, _value);
      _state = MJ_NEWSTATE(MJ_STATE_PRE_DONE);
      parse(ch); // parse this character again.
    }

  } else if (_state == MJ_STATE_Q_VALUE) {
    if (ch == '\\') {
      _state = MJ_NEWSTATE(MJ_STATE_Q_VALUE_ESC);
      _esc[0] = NUL;

    } else if (ch != '"') {
      strncat(_value, ch, sizeof(_value));

    } else {
      // LOGGER_TRACE(" value=%s", _value);
      if (_callbackFn)
        (_callbackFn)(_level, _path, _name, _value);
      _state = MJ_NEWSTATE(MJ_STATE_PRE_DONE);
    }

  } else if (_state == MJ_STATE_Q_VALUE_ESC) {
    // single char escape sequences
    if (ch == 'b') {
      ch = '\b';
    } else if (ch == 'f') {
      ch = '\f';
    } else if (ch == 'n') {
      ch = '\n';
    } else if (ch == 'r') {
      ch = '\r';
    } else if (ch == 't') {
      ch = '\t';
    } else if (ch == 'u') {
      // read 4 hex digits  \u0034
      _state = MJ_NEWSTATE(MJ_STATE_Q_VALUE_ESCU);
      _esc[0] = NUL;

    } else {
      // like \" , \\, \/ and all other normal chars.
    } // if

    if (_state != MJ_STATE_Q_VALUE_ESCU) {
      strncat(_value, ch, sizeof(_value));
      _state = MJ_STATE_Q_VALUE;
    } // if

  } else if (_state == MJ_STATE_Q_VALUE_ESCU) {
    strncat(_esc, ch, sizeof(_esc));
    if (strlen(_esc) == 4) {
      long l = strtol(_esc, NULL, 16);
      ch = (char)(l % 0x00FF);
      strncat(_value, ch, sizeof(_value));
      _state = MJ_NEWSTATE(MJ_STATE_Q_VALUE);
    } // if

  } else if (_state == MJ_STATE_PRE_DONE) {
    if (ch == ',') {
      _name[0] = _value[0] = NUL;
      _state = MJ_NEWSTATE(MJ_STATE_PRE_NAME);

    } else if (ch == '}') {
      if (_level > 2) {
        // remove last path element
        char *p = strrchr(_path, MICROJSON_PATH_SEPARATOR);
        *p = NUL;
        _level--;

      } else if (_level > 1) {
        // remove last path element
        _path[0] = NUL;
        _level--;

      } else if (_level == 1) {
        _level == 0;
        _path[0] = NUL;
        _state = MJ_NEWSTATE(MJ_STATE_DONE);
      } else {
        _state = MJ_ERROR("to many '}'");
      }
    }
  }
} // parse()

// end.
