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

#include <Board.h>
#include <Element.h>
#include <ElementRegistry.h>
#include <string.h>

#include "MicroJsonParser.h"

#include "sntp.h"

// time_t less than this value is assumed as not initialized.
#define MIN_VALID_TIME (30 * 24 * 60 * 60)

extern const char *ssid;
extern const char *passPhrase;

#define NetMode_AUTO 3
#define NetMode_PSK 2
#define NetMode_PASS 1

#include <DNSServer.h>

DNSServer dnsServer;
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

const byte DNS_PORT = 53;

/**
 * @brief Initialize a blank board.
 */
void Board::init(ESP8266WebServer *serv)
{
  LOGGER_TRACE("init()");
  server = serv;
  sysLED = -1; // configured by device-element
  sysButton = -1; // configured by device-element
  boardState = BOARDSTATE_NONE;

  WiFi.begin();
  deviceName = WiFi.hostname(); // use mac based default device name
  deviceName.replace("_", ""); // Underline in hostname is not conformant, see
                               // https://tools.ietf.org/html/rfc1123 952

  // check save-mode
  savemode = false;

} // init()


/**
 * @brief Add and config the Elements defined in the config files.
 *
 */
void Board::addElements()
{
  LOGGER_TRACE("addElements()");
  Element *_lastElem = NULL; // last created Element
  MicroJson *mj;

  mj = new MicroJson(
      [this, &_lastElem](int level, char *path, char *name, char *value) {
        // LOGGER_TRACE("callback %d %s", level, path);

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


void Board::start(Element_StartupMode startupMode)
{
  LOGGER_TRACE("start(%d)", startupMode);

  // make elements active that match
  Element *l = _list2;
  while (l != NULL) {

    if ((!l->active) && (l->startupMode <= startupMode)) {
      // start element when not already active
      LOGGER_TRACE("starting %s...", l->id);
      l->start();
    } // if

    l = l->next;
  } // while

  active = true;
  _next2 = NULL;
} // start()


// switch to a new state
void Board::_newState(BoardState newState)
{
  LOGGER_TRACE("BoardState = %d", newState);
  boardState = newState;
}

// loop next element, only one at a time!
void Board::loop()
{
  unsigned long now = millis();
  wl_status_t wifi_status = WiFi.status();
  bool autoCon = WiFi.getAutoConnect();
  String ipStr;

  switch (boardState) {
  case BOARDSTATE_NONE:
    _newState(BOARDSTATE_LOAD);
    break;

  case BOARDSTATE_LOAD:
    // load all config files and create+start elements
    addElements();
    start(Element_StartupMode::System);
    // LOGGER_TRACE("SYS Elements started.");

    LOGGER_INFO("devicename=%s", deviceName.c_str());
    WiFi.hostname(deviceName);

    if (display) {
      display->drawText(0, 0, 0, "HomeDing...");
      display->flush();
    } // if

    netMode = NetMode_AUTO;

    // detect no configured network situation
    if ((WiFi.SSID().length() == 0) && (strlen(ssid) == 0)) {
      // start hotspot right now.
      _newState(BOARDSTATE_STARTCAPTIVE);
    } else {
      _newState(BOARDSTATE_CONNECT);
    }

    // wait at least 6 seconds for offering config mode
    configPhaseEnd = now + nextModeTime;
    break;

  case BOARDSTATE_CONNECT:
    LOGGER_TRACE("autoconnect=%d", autoCon);

    // no accesspoint network when starting up normally.
    WiFi.softAPdisconnect(true);

    if ((!autoCon) && (netMode == NetMode_AUTO))
      netMode = NetMode_PSK;

    // connect to a same network as before ?
    LOGGER_TRACE("wifi status=(%d)", wifi_status);
    LOGGER_TRACE("wifi ssid=(%s)", WiFi.SSID().c_str());

    if (netMode == NetMode_AUTO) {
      // 1. priority:
      // give autoconnect the chance to do it.
      // works only after a successfull network connection in the past.
      LOGGER_TRACE("NetMode_AUTO");
      WiFi.mode(WIFI_STA);

    } else if (netMode == NetMode_PSK) {
      // 2. priority:
      // explicit connect with the saved passwords.
      // works only after a successfull network connection in the past.
      LOGGER_TRACE("NetMode_PSK");
      WiFi.mode(WIFI_STA);
      WiFi.begin();

    } else if (netMode == NetMode_PASS) {
      // 3. priority:
      // use fixed network and passPhrase known at compile time.
      // works only after a successfull network connection in the past.
      if (*ssid) {
        LOGGER_TRACE("NetMode_PASS: %s %s", ssid, passPhrase);
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, passPhrase);
      } else {
      }
    } // if

    // Enable sysLED for blinking while waiting for network or config mode
    if (sysLED >= 0) {
      pinMode(sysLED, OUTPUT);
      digitalWrite(sysLED, HIGH);
    }

    // Enable sysButton for entering config mode
    if (sysButton >= 0) {
      pinMode(sysButton, INPUT_PULLUP);
    }

    // wait max 30 seconds for connecting to the network
    connectPhaseEnd = now + captiveTime;

    _newState(BOARDSTATE_CONFWAIT);
    break;
  case BOARDSTATE_CONFWAIT:
  case BOARDSTATE_WAIT:
    // make sysLED blink
    if (sysLED >= 0) {
      digitalWrite(sysLED, ((configPhaseEnd - now) % 700) > 350 ? HIGH : LOW);
    } // if

    // check sysButton
    if (sysButton >= 0) {
      if (digitalRead(sysButton) == LOW) {
        _newState(BOARDSTATE_STARTCAPTIVE);
      }
    }

    if (boardState == BOARDSTATE_CONFWAIT) {
      if (now > configPhaseEnd)
        _newState(BOARDSTATE_WAIT);

    } else {
      if (wifi_status == WL_CONNECTED) {
        LOGGER_TRACE("connected.");
        _newState(BOARDSTATE_GREET);

      } else if ((wifi_status == WL_NO_SSID_AVAIL) ||
                 (wifi_status == WL_CONNECT_FAILED) ||
                 (now >= connectPhaseEnd)) {
        netMode -= 1;
        LOGGER_TRACE("next connect method = %d", netMode);
        if (netMode) {
          _newState(BOARDSTATE_CONNECT);
        } else {
          LOGGER_INFO("no-net restarting...");
          delay(10000);
          ESP.restart();
        }
      } else {
        delay(100);
      }
    } // if (BOARDSTATE_WAIT)
    break;

  case BOARDSTATE_GREET:
    ipStr = WiFi.localIP().toString();
    LOGGER_TRACE("Connected to: %s", WiFi.SSID().c_str());
    LOGGER_TRACE("  as: %s", WiFi.hostname().c_str());
    LOGGER_TRACE("  IP: %s", ipStr.c_str());
    LOGGER_TRACE(" MAC: %s", WiFi.macAddress().c_str());

    if (display) {
      display->clear();
      display->drawText(0, 0, 0, deviceName);
      display->drawText(0, display->lineHeight, 0, ipStr.c_str());
      display->flush();
      delay(1600);
      display->clear();
      display->flush();
    } // if

    // release sysLED
    if (sysLED >= 0) {
      digitalWrite(sysLED, HIGH);
      pinMode(sysLED, INPUT);
    }

    server->begin();
    start(Element_StartupMode::Network);
    _newState(BOARDSTATE_RUN);
    break;

  case BOARDSTATE_RUN:
    if (!validTime) {
      // check if time is valid now -> start all elements with
      // Element_StartupMode::Time
      time_t current_stamp = getTime();
      if (current_stamp) {
        start(Element_StartupMode::Time);
        validTime = true;
        return;
      } // if
    }

    // dispatch next action if any
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
      return;
    } // if

    // give some time to next active element
    if (_next2 == NULL) {
      _next2 = _list2;
    } // if
    if (_next2) {
      if (_next2->active) {
        _next2->loop();
      }
      _next2 = _next2->next;
    } // if
    break;

  case BOARDSTATE_STARTCAPTIVE:
    LOGGER_INFO("start captive portal...");

    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP("HomeDing");
    delay(500);
    LOGGER_INFO(" AP-IP: %s", WiFi.softAPIP().toString().c_str());

    if (sysLED >= 0)
      digitalWrite(sysLED, LOW);

    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(DNS_PORT, "*", apIP);

    server->begin();

    _newState(BOARDSTATE_RUNCAPTIVE);
    _captiveEnd = now + (5 * 60 * 1000);
    break;

  case BOARDSTATE_RUNCAPTIVE:
    // server.handleClient(); needs to be called in main loop.
    dnsServer.processNextRequest();

    if (now > _captiveEnd)
      reboot(false);
    break;

    // default:
    //   break;
  } // switch

} // loop()


Element *Board::findById(String &id)
{
  return (findById(id.c_str()));
}

Element *Board::findById(const char *id)
{
  // LOGGER_TRACE("findById(%s)", id);

  Element *l = _list2;
  while (l != NULL) {
    if (strcmp(l->id, id) == 0) {
      // LOGGER_TRACE(" found:%s", l->id);
      break; // while
    } // if
    l = l->next;
  } // while
  return (l);
} // findById


void Board::_dispatchSingle(String evt)
{
  // LOGGER_TRACE("dispatchSingle(%s)", evt.c_str());

  int pos1 = evt.indexOf(ELEM_PARAMETER);
  int pos2 = evt.indexOf(ELEM_VALUE);

  if (pos1 <= 0) {
    LOGGER_ERR("No action given: %s", evt.c_str());

  } else {
    String targetName, name, value;

    targetName = evt.substring(0, pos1);
    targetName.toLowerCase();

    Element *target = findById(targetName);

    if (!target) {
      LOGGER_ERR("dispatch target %s not found", targetName.c_str());

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
void Board::dispatch(String &action, int value)
{
  if (action.length() > 0) {
    char tmp[16];
    itoa(value, tmp, 10);
    dispatch(action.c_str(), tmp);
  } // if

} // dispatch


/**
 * @brief prepare sending all the actions to the right elements.
 */
void Board::dispatch(String &action, const char *value)
{
  if (action.length() > 0)
    dispatch(action.c_str(), value);
} // dispatch


void Board::dispatch(const char *action, const char *value)
{
  if ((action != NULL) && (*action)) {
    String tmp = action;
    if (value != NULL)
      tmp.replace("$v", value);

    // append to existing _actionList
    if (_actionList.length() > 0)
      _actionList.concat(ACTION_SEPARATOR);
    _actionList.concat(tmp);
  } // if
} // dispatch


void Board::getState(String &out, const String &path)
{
  // LOGGER_TRACE("getState(%s)", path.c_str());
  String ret = "{";
  const char *cPath = path.c_str();

  Element *l = _list2;
  while (l != NULL) {
    // LOGGER_TRACE("  ->%s", l->id);
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

  // prettify somehow
  ret.replace("},", "},\n");

  out = ret;
} // getState


void Board::setState(String &path, const String &property, const String &value)
{
  // LOGGER_TRACE("setState(%s, %s, %s)", path.c_str(), property.c_str(),
  // value.c_str());
  Element *e = findById(path);
  if (e != NULL) {
    e->set(property.c_str(), value.c_str());
  }
}

// ===== Time functionality =====

unsigned long Board::getSeconds()
{
  return (millis() / 1000);
}


// return the seconds since 1.1.1970 00:00:00
time_t Board::getTime()
{
  uint32 current_stamp = sntp_get_current_timestamp();
  if (current_stamp <= MIN_VALID_TIME) {
    current_stamp = 0;
  } // if
  return ((time_t)(current_stamp));
} // getTime()


// return the seconds of today.
time_t Board::getTimeOfDay()
{
  uint32 current_stamp = sntp_get_current_timestamp();
  if (current_stamp > MIN_VALID_TIME) {
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
  // LOGGER_TRACE("getElement(%s)", elementTypeName);

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
  // LOGGER_TRACE("found: %d", l);
  return (l);
} // getElement


void Board::reboot(bool wipe)
{
  LOGGER_INFO("reboot...");
  if (wipe)
    WiFi.disconnect(true);
  delay(1000);
  ESP.reset();
};

/**
 * @brief Add another element to the board into the list of created elements.
 */
void Board::_add(const char *id, Element *e)
{
  // LOGGER_TRACE("_add(%s)", id);

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
  } // if
  e->init(this);
} // _add()

// End
