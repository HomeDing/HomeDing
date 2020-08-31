/**
 * @file MicroJsonParser.cpp
 * @brief Streamin JSON Parser.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license.
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see MicroJsonParser.h
 */
#include <Arduino.h>
#include <MicroJsonParser.h>

#include <core/Logger.h>

#include <FS.h>

// enable MJ_TRACEDETAILS to get some very detailled output on parsing
// #define MJ_TRACEDETAILS

// Remarks:
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
#define MJ_STATE_NUM_VALUE (0x25)

// Arrays
#define MJ_STATE_PRE_ITEM (0x42 + MJ_IGNOREBLANCS)
#define MJ_STATE_POST_ITEM (0x43 + MJ_IGNOREBLANCS)

#define MJ_STATE_PRE_DONE (0x31 + MJ_IGNOREBLANCS)
#define MJ_STATE_DONE (0x32 + MJ_IGNOREBLANCS)
#define MJ_STATE_ERROR (0x50 + MJ_IGNOREBLANCS)

#define NUL '\0'

#define MJ_OBJECTLEVEL (-1)
#define MJ_ARRAYLEVEL (0)

#if defined(MJ_TRACEDETAILS)
// DEBUG State transitions
#define MJ_NEWSTATE(newState) (LOGGER_INFO("state %x %d:<%s>:", newState & 0xFF, _level, _path), delay(1), newState)
#define MJ_TRACE(...) LOGGER_RAW(__VA_ARGS__)
#else
#define MJ_NEWSTATE(newState) (newState)
#define MJ_TRACE(...)
#endif

#define MJ_ERROR(reason) (LOGGER_ERR(reason), MJ_STATE_ERROR)


MicroJson::MicroJson(MicroJsonCallbackFn callback)
{
  _callbackFn = callback;
  _path[0] = NUL;
  _name[0] = NUL;
  _value[0] = NUL;
  _esc[0] = NUL;
  init();
};


void MicroJson::init()
{
  _state = MJ_STATE_INIT;
  __level = 0;
  _index[0] = MJ_OBJECTLEVEL;
}


void MicroJson::parse(const char *s)
{
  if (_callbackFn) {
    parseChar(s);
  } // if
};


void MicroJson::parseFile(const char *fName)
{
  char buffer[128 + 1]; // one extra char for enforced NUL char.
  size_t len;

  init();

  if (_callbackFn) {
    if (SPIFFS.exists(fName)) {
      MJ_TRACE("parsing file %s", fName);

      File file = SPIFFS.open(fName, "r");
      while (file.available()) {
        len = file.readBytes(buffer, sizeof(buffer) - 1);
        if (len) {
          buffer[len] = NUL;
          parseChar(buffer);
        }
      }
      file.close();
    } // if
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


void MicroJson::parseChar(const char *s)
{
  if (s != NULL) {
    while (*s != '\0') {
      if (parseChar(*s))
        s++;
    } // while
  }
} // parseChar()


/**
 * @brief get the next character from the input and parse.
 */
bool MicroJson::parseChar(char ch)
{
  bool ret = true; // in most cases
  int _level = __level;

  // create some debug output on relevant input characters
  // if (!(_state & MJ_IGNOREBLANCS) || !isspace(ch)) {
  //   MJ_TRACE("%02x> (%c)", _state, ch);
  // }

  if ((_state & MJ_IGNOREBLANCS) && isspace(ch)) {
    // ignore white space here.
    // MJ_TRACE(" ignored");

  } else if (_state == MJ_STATE_ERROR) {
    // stay in error status
    // LOGGER_RAW(" -err");

  } else if (_state == MJ_STATE_DONE) {
    // already finished, anything is error
    _state = MJ_ERROR("unexpected character at end");

  } else if (_state == MJ_STATE_INIT) {
    if (ch != '{') {
      _state = MJ_ERROR("'{' expected");
    } else {
      _level++;
      _index[_level] = MJ_OBJECTLEVEL;
      _state = MJ_NEWSTATE(MJ_STATE_PRE_NAME);
      // (_callbackFn)(_level, _path, NULL); // empty not required
    }

  } else if (_state == MJ_STATE_PRE_NAME) {
    if (ch == '"') {
      _name[0] = _value[0] = NUL;
      _state = MJ_NEWSTATE(MJ_STATE_Q_NAME);
    } else {
      _state = MJ_NEWSTATE(MJ_STATE_PRE_DONE);
      ret = false; // parse this character again.
    }

  } else if (_state == MJ_STATE_Q_NAME) {
    if (ch != '"') {
      strncat(_name, ch, sizeof(_name));
    } else {
      _state = MJ_NEWSTATE(MJ_STATE_ASSIGN);
    } // if

  } else if (_state == MJ_STATE_ASSIGN) {
    if (ch == ':') {
      _state = MJ_NEWSTATE(MJ_STATE_PRE_VALUE);
    } else {
      _state = MJ_ERROR("':' expected");
    }


  } else if (_state == MJ_STATE_PRE_ITEM) {
    // new item in array: add [x] to path
    char tmp[16];
    itoa(_index[_level], tmp, 10);
    strncat(_path, '[', sizeof(_path));
    strncat(_path, tmp, sizeof(_path));
    strncat(_path, ']', sizeof(_path));
    MJ_TRACE("array item: %s", _path);

    if (ch == '{') {
      // open object level
      _level++;
      _index[_level] = MJ_OBJECTLEVEL;
      _state = MJ_NEWSTATE(MJ_STATE_PRE_NAME);

    } else {
      // just a value
      _name[0] = NUL;
      _state = MJ_NEWSTATE(MJ_STATE_PRE_VALUE);
      ret = false; // parse this character again.
    }

  } else if (_state == MJ_STATE_POST_ITEM) {

    if (ch == ',') {
      // another item in the array
      _index[_level]++;
      _state = MJ_NEWSTATE(MJ_STATE_PRE_ITEM);

    } else if (ch == ']') {
      // close array
      if (_level > 2) {
        *strrchr(_path, MICROJSON_PATH_SEPARATOR) = NUL;
      } else {
        _path[0] = NUL;
      }
      _level--;
      _state = MJ_NEWSTATE(MJ_STATE_PRE_DONE);
    }

  } else if (_state == MJ_STATE_PRE_VALUE) {
    _value[0] = NUL;
    if (ch == '"') {
      // quoted value / string
      _state = MJ_NEWSTATE(MJ_STATE_Q_VALUE);

    } else if (ch == '{') {
      if (_path[0] != NUL)
        strncat(_path, MICROJSON_PATH_SEPARATOR, sizeof(_path));
      strncat(_path, _name, sizeof(_path));
      (_callbackFn)(_level, _path, NULL);

      // nested object
      _level++;
      _index[_level] = MJ_OBJECTLEVEL;

      _state = MJ_NEWSTATE(MJ_STATE_PRE_NAME);

    } else if (ch == '[') {
      // open array
      MJ_TRACE("array open level %d", _level + 1);
      if (_path[0] != NUL)
        strncat(_path, MICROJSON_PATH_SEPARATOR, sizeof(_path));
      strncat(_path, _name, sizeof(_path));
      (_callbackFn)(_level, _path, NULL);

      _level++;
      _index[_level] = MJ_ARRAYLEVEL;

      _state = MJ_NEWSTATE(MJ_STATE_PRE_ITEM);


    } else if (isdigit(ch)) {
      _state = MJ_NEWSTATE(MJ_STATE_NUM_VALUE);
      ret = false; // parse this character again.
    }

  } else if (_state == MJ_STATE_NUM_VALUE) {
    if (isdigit(ch) || (ch == '.')) {
      strncat(_value, ch, sizeof(_value));

    } else {
      // this character doesn't belong to the value any more.
      // LOGGER_RAW(" value=%s", _value);
      strncat(_path, MICROJSON_PATH_SEPARATOR, sizeof(_path));
      strncat(_path, _name, sizeof(_path));
      (_callbackFn)(_level, _path, _value);
      *strrchr(_path, MICROJSON_PATH_SEPARATOR) = NUL;
      _state = MJ_NEWSTATE(MJ_STATE_PRE_DONE);
      ret = false; // parse this character again.
    }
  } else if (_state == MJ_STATE_Q_VALUE) {
    if (ch == '\\') {
      _state = MJ_NEWSTATE(MJ_STATE_Q_VALUE_ESC);
      _esc[0] = NUL;

    } else if (ch != '"') {
      strncat(_value, ch, sizeof(_value));

    } else {
      // quoted value completed
      strncat(_path, MICROJSON_PATH_SEPARATOR, sizeof(_path));
      strncat(_path, _name, sizeof(_path));
      (_callbackFn)(_level, _path, _value);
      *strrchr(_path, MICROJSON_PATH_SEPARATOR) = NUL;
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
      long l = strtol(_esc, nullptr, 16);
      ch = (char)(l % 0x00FF);
      strncat(_value, ch, sizeof(_value));
      _state = MJ_NEWSTATE(MJ_STATE_Q_VALUE);
    } // if

  } else if (_state == MJ_STATE_PRE_DONE) {
    // after a property-key/value.

    if (ch == ',') {
      // new object property expected
      if (_index[_level] == MJ_OBJECTLEVEL) {
        _state = MJ_NEWSTATE(MJ_STATE_PRE_NAME);

      } else {
        // next array item: remove last [x] from path
        MJ_TRACE("array next item");
        *strrchr(_path, '[') = NUL;
        _index[_level]++;
        _state = MJ_NEWSTATE(MJ_STATE_PRE_ITEM);
      }

    } else if (ch == ']') {
      // close array
      MJ_TRACE("array close.");
      _level--;
      // remove last path element
      *strrchr(_path, MICROJSON_PATH_SEPARATOR) = NUL;
      // _state = MJ_NEWSTATE(MJ_STATE_PRE_DONE); // stay in the same state

    } else if (ch == '}') {
      MJ_TRACE("object close. %d", _level);
      // close the object
      _level--;
      if (_index[_level] != MJ_OBJECTLEVEL) {
        // array item was closed: remove last [x] from path
        MJ_TRACE("array next item");
        *strrchr(_path, '[') = NUL;
        _state = MJ_NEWSTATE(MJ_STATE_POST_ITEM);

      } else if (_level > 1) {
        // remove last path element
        *strrchr(_path, MICROJSON_PATH_SEPARATOR) = NUL;
        // _state = MJ_NEWSTATE(MJ_STATE_PRE_DONE); // stay in the same state

      } else if (_level == 1) {
        // remove last path element
        _path[0] = NUL;

      } else if (_level == 0) {
        _state = MJ_NEWSTATE(MJ_STATE_DONE);
      } else {
        _state = MJ_ERROR("to many '}'");
      }
    } // if
  }
  __level = _level;
  return (ret);
} // parseChar()

// end.
