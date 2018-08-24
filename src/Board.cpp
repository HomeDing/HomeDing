/**
 * @file board.cpp
 *
 * @brief Implementation of the Board class for the HomeDing Library
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog: see Board.h
 */

#include "Board.h"
#include "Element.h"
#include "ElementRegistry.h"

#include "MicroJsonParser.h"

#define LOGGER_MODULE "board"
#define LOGGER_LEVEL 0
#include "Logger.h"

#include "sntp.h"

/**
 * @brief initialize the board.
 */
void Board::init(DisplayAdapter *d, ESP8266WebServer *s)
{
  LOGGER_TRACE("init()");
  display = d;
  server = s;
} // init()


void Board::addElements()
{
  LOGGER_TRACE("addElements()");
  Element *_lastElem = NULL; // last created Element
  MicroJson *mj;

  mj = new MicroJson(
    [this, &_lastElem](int level, char *path, char *name, char *value) {
      if (level == 3) {
        if (name == NULL) {
          LOGGER_TRACE(" new Element %s", path);
          // path = <elem-type>/<elem-name>

          // create that element using the typename
          char tmp[32];
          strncpy(tmp, path, sizeof(tmp));
          tmp[31] = '\0'; // force termination
          char *p = strchr(tmp, MICROJSON_PATH_SEPARATOR);
          if (p)
            *p = '\0'; // cut at first path separator. The type remains in the
                        // buffer.

          _lastElem = ElementRegistry::createElement(tmp);
          if (_lastElem == NULL) {
            LOGGER_ERR("Cannot create Element type %s", tmp);

          } else {
            // add to the list of elements
            _add(path, _lastElem);
          } // if

        } else if (_lastElem != NULL) {
          // add a parameter to the last Element
          LOGGER_TRACE(" add %s=%s", name, value);
          _lastElem->set(name, value);
        } // if
      } // if
    });

  // config the thing to the local network
  mj->parseFile(ENV_FILENAME);

  // config the Elements of the thing
  mj->parseFile(CONF_FILENAME);

} // addElements()


void Board::start()
{
  LOGGER_TRACE("start()");

  // make elements active
  Element *l = _list2;
  while (l != NULL) {
    LOGGER_TRACE("starting %s...", l->id);
    l->start();
    l = l->next;
  } // while
  active = true;
  _next2 = NULL;
} // start()


// loop next element, only one at a time!
void Board::loop()
{
  if (active) {

    if (_actionList.length() > 0) {
      String _lastAction;

      // extract first action
      int pos = _actionList.indexOf(ACTION_SEPARATOR);
      if (pos > 0) {
        _lastAction = _actionList.substring(0, pos);
        _actionList.remove(0, pos + 1);
      } else {
        _lastAction = _actionList;
        _actionList = (const char *)NULL;
      } // if
      _dispatchSingle(_lastAction);
    } // if

    yield();

    if (_next2 == NULL) {
      _next2 = _list2;
    } // if
    if (_next2) {
      if (_next2->active) {
        _next2->loop();
      }
      _next2 = _next2->next;
    } // if
  } // if
} // loop()


Element *Board::findById(String &id)
{
  return (findById(id.c_str()));
}

Element *Board::findById(const char *id)
{
  LOGGER_TRACE("findById(%s)", id);

  Element *l = _list2;
  while (l != NULL) {
    if (strcmp(l->id, id) == 0) {
      LOGGER_TRACE(" found:%s", l->id);
      break; // while
    } // if
    l = l->next;
  } // while
  return (l);
} // findById


void Board::_dispatchSingle(String evt)
{
  LOGGER_TRACE("dispatchSingle(%s)", evt.c_str());

  int pos1 = evt.indexOf(ELEM_PARAMETER);
  int pos2 = evt.indexOf(ELEM_VALUE);

  if (pos1 <= 0) {
    LOGGER_ERR("No action given");

  } else {
    String targetName, name, value;

    targetName = evt.substring(0, pos1);
    targetName.toLowerCase();

    Element *target = findById(targetName);

    if (!target) {
      LOGGER_ERR("target %s not found", targetName.c_str());

    } else {
      if (pos2 > 0) {
        name = evt.substring(pos1 + 1, pos2);
        value = evt.substring(pos2 + 1);
      } else {
        name = evt.substring(pos1 + 1);
        value = "";
      }
      bool ret = target->set(name.c_str(), value.c_str());
      if (!ret)
        LOGGER_ERR("Event '%s' was not handled", evt.c_str());
    }
  }
} // _dispatchSingle()


/**
 * @brief prepare sending all the actions to the right elements.
 */
void Board::dispatch(String &action, const char *value)
{
  dispatch(action.c_str(), value);
} // dispatch


void Board::dispatch(const char *action, const char *value)
{
  String tmp = action;

  if (value != NULL)
    tmp.replace("$v", value);

  // append to existing _actionList
  if (_actionList.length() > 0)
    _actionList.concat(ACTION_SEPARATOR);
  _actionList.concat(tmp);
} // dispatch


void Board::getState(String &out, String path)
{
  LOGGER_TRACE("getState(%s)", path.c_str());
  String ret = "{";
  const char *cPath = path.c_str();

  Element *l = _list2;
  while (l != NULL) {
    LOGGER_TRACE("  ->%s\n", l->id);
    if ((cPath[0] == '\0') || (strcmp(l->id, cPath) == 0)) {
      ret += "\"";
      ret += l->id;
      ret += "\": {";
      l->pushState([&ret](const char *name, const char *value) {
        // LOGGER_TRACE("->%s=%s", name, value);
        ret.concat('\"');
        ret.concat(name);
        ret.concat("\":\"");
        ret.concat(value);
        ret.concat("\",");
      });

      // remove last comma
      if (ret.charAt(ret.length() - 1) == ',') {
        ret.remove(ret.length() - 1);
      }
      ret += "},";
    } // if

    l = l->next;
  } // while

  // remove last comma
  if (ret.charAt(ret.length() - 1) == ',') {
    ret.remove(ret.length() - 1);
  }

  // close root object
  ret += "}";

  // pretify somehow
  ret.replace("},", "},\n");

  out = ret;
} // getState


void Board::setState(String &path, String property, String value)
{
  LOGGER_TRACE("setState(%s, %s, %s)", path.c_str(), property.c_str(),
               value.c_str());
  Element *e = findById(path);
  if (e != NULL) {
    e->set(property.c_str(), value.c_str());
  }
}

// ===== Time functionality =====

// fill the time structure from a timestamp;
void Board::getTime(struct tm *time)
{
  uint32 current_stamp = sntp_get_current_timestamp();
  if (current_stamp > (24 * 60 * 60)) {
    struct tm *tmp = localtime((time_t *)(&current_stamp));
    memcpy(time, tmp, sizeof(struct tm));
  } else {
    memset(time, 0, sizeof(struct tm));
  }
} // getTime()


// return the seconds of today.
time_t Board::getTimeOfDay()
{
  uint32 current_stamp = sntp_get_current_timestamp();
  if (current_stamp > (24 * 60 * 60)) {
    current_stamp = current_stamp % (24 * 60 * 60);
  } else {
    current_stamp = 0;
  }
  return ((time_t)(current_stamp));
} // getTimeOfDay()

/**
 * @brief Get a Element by typename. Returns the first found element.
 */
Element *Board::getElement(const char *elementTypeName)
{
  LOGGER_TRACE("getElement(%s)", elementTypeName);

  String tn = elementTypeName;
  tn.concat(ELEM_ID_SEPARATOR);
  int tnLength = tn.length();

  Element *l = _list2;
  while (l != NULL) {
    if (String(l->id).substring(0, tnLength).equalsIgnoreCase(tn)) {
      break; // while
    } // if
    l = l->next;
  } // while
  LOGGER_TRACE("found: %d", l);
  return (l);
} // getElement


// add another single element to the board.
void Board::_add(const char *id, Element *e)
{
  LOGGER_TRACE("_add(%s)", id);

  strcpy(e->id, id);
  e->next = NULL;
  Element *l = _list2;

  // append to end of list.
  if (l == NULL) {
    // first Element.
    _list2 = e;
  } else {
    // search last Element.
    while (l->next != NULL)
      l = l->next;
    l->next = e;
    e->init(this);
  } // if
} // _add()
// End
