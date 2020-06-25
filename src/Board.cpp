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

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include <ElementRegistry.h>

#include "MicroJsonParser.h"

#include "sntp.h"

#include <DNSServer.h>

#define TRACE(...)
// #define TRACE(...) LOGGER_INFO(__VA_ARGS__)

// time_t less than this value is assumed as not initialized.
#define MIN_VALID_TIME (30 * 24 * 60 * 60)

#define NetMode_PASS 1
#define NetMode_PSK 2
#define NetMode_AUTO 3

DNSServer dnsServer;
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

const byte DNS_PORT = 53;

extern const char *ssid;
extern const char *passPhrase;


// ===== detect request for network config and non-save mode using 2 resets in less than 2 sec.
// this is implemented by using the RTC memory at offset with a special value flag

/** The offset of the 4 byte signature, must be a multiple of 4 */
#define NETRESET_OFFSET 3

/** The magic 4-byte number indicating that a reset has happened before, "RST". Byte 4 is the counter */
#define NETRESET_FLAG ((uint32)0x52535400)
#define NETRESET_MASK ((uint32)0xFFFFFF00)
#define NETRESET_VALUEMASK ((uint32)0x00000000FF)


// get the number of resets in the last seconds using rtc memory for counting.
int getResetCount()
{
  // LOGGER_TRACE("getResetCount()");
  int res = 0;
  uint32 netResetValue;

  // check NETRESET_FLAG
  ESP.rtcUserMemoryRead(NETRESET_OFFSET, &netResetValue, sizeof(netResetValue));
  // LOGGER_TRACE("RESET read %08x", netResetValue);

  if ((netResetValue & NETRESET_MASK) == NETRESET_FLAG) {
    // reset was pressed twice.
    // LOGGER_TRACE("RESET detected.");
    netResetValue++;
    res = (netResetValue & 0x000000FF);
  } else {
    netResetValue = NETRESET_FLAG;
  }

  // store NETRESET_FLAG and counter to rtc memory
  ESP.rtcUserMemoryWrite(NETRESET_OFFSET, &netResetValue, sizeof(netResetValue));
  return (res);
} // getResetCount()


// get the number of resets in the last seconds using rtc memory for counting.
void clearResetCount()
{
  LOGGER_TRACE("clearResetCount");
  uint32 netResetValue = 0;
  ESP.rtcUserMemoryWrite(NETRESET_OFFSET, &netResetValue, sizeof(netResetValue));
}

/**
 * @brief Initialize a blank board.
 */
void Board::init(ESP8266WebServer *serv)
{
  server = serv;
  sysLED = -1; // configured by device-element
  sysButton = -1; // configured by device-element
  boardState = BOARDSTATE_NONE;

  WiFi.begin();
  deviceName = WiFi.hostname(); // use mac based default device name
  deviceName.replace("_", ""); // Underline in hostname is not conformant, see
      // https://tools.ietf.org/html/rfc1123 952

  _resetCount = getResetCount();
  LOGGER_INFO("RESET # %d", _resetCount);

  // disable savemode when rebooting twice in a row
  savemode = (!_resetCount);
  LOGGER_INFO("savemode=%d", savemode);

} // init()


void Board::_checkNetState()
{
  delay(0);
  wl_status_t newState = WiFi.status();
  if (newState != _wifi_status) {
    LOGGER_RAW("new netstate: %d", newState);
    _wifi_status = newState;
  }
}


// Remark: some settings on the board class can be modified using the device element.
// see deviceElement.cpp


/**
 * @brief Add and config the Elements defined in the config files.
 *
 */
void Board::addElements()
{
  TRACE("addElements()");
  Element *_lastElem = NULL; // last created Element
  MicroJson *mj;

  mj = new MicroJson(
      [this, &_lastElem](int level, char *path, char *value) {
        TRACE("callback %d %s =%s", level, path, value ? value : "-");
        _checkNetState();

        if (level == 1) {

        } else if (level == 2) {
          LOGGER_TRACE("new %s", path);
          // extract type name
          char typeName[32];

          char *p = path;
          char *t = typeName;
          while (*p && *p != MICROJSON_PATH_SEPARATOR) {
            *t++ = *p++;
          } // while
          *t = '\0';

          _lastElem = ElementRegistry::createElement(typeName);
          if (_lastElem == NULL) {
            LOGGER_ERR("Cannot create Element type %s", typeName);

          } else {
            // add to the list of elements
            _add(path, _lastElem);
          } // if

        } else if ((level > 2) && (_lastElem != NULL)) {
          char *name = strrchr(path, MICROJSON_PATH_SEPARATOR) + 1;
          LOGGER_TRACE(" %s=%s", name, value ? value : "-");
          // add a parameter to the last Element
          // LOGGER_TRACE(" %s:%s", name, value);
          _lastElem->set(name, value);
        } // if
      });

  // config the thing to the local network
  mj->parseFile(ENV_FILENAME);
  _checkNetState();

  // config the Elements of the thing
  mj->parseFile(CONF_FILENAME);
  _checkNetState();

  // mj->parse(R"==({
  //   "sli": { "0": {
  //     "description" : "Listen for commands on the Serial in line"
  //   }}})==");

#if 0

  LOGGER_INFO("[[START...");
mj = new MicroJson(
    [this](int level, char *path, char *name, char *value) {
      if (name && value) {
        // LOGGER_INFO("[[ <%s/%s>=\"%s\"", path, name, value);

        String p(path);
        p.concat('/');
        p.concat(name);
        if (p.equalsIgnoreCase("list[1]/main/temp")) {
          LOGGER_INFO("next temp=%s", value);
        }
      }
    });

  mj->parse(R"==({
"cod" : "200","message" : 0,"cnt" : 40,
"list" : [
  {"dt" : 1578679200,"main" : {"temp" : 277.96,"feels_like" : 274.42,"temp_min" : 277.96,"temp_max" : 280.11,"pressure" : 1025,"sea_level" : 1025,"grnd_level" : 1010,"humidity" : 75,"temp_kf" : -2.15},"weather" : [{"id" : 500,"main" : "Rain","description" : "light rain","icon" : "10n"}],"clouds" : {"all" : 64},"wind" : {"speed" : 2.38,"deg" : 287},"rain" : {"3h" : 0.06},"sys" : {"pod" : "n"},"dt_txt" : "2020-01-10 18:00:00"},
  {"dt" : 1578690000,
"main" : {
"temp" : 277.66,
"feels_like" : 273.73,
"temp_min" : 277.66,
"temp_max" : 279.27,
"pressure" : 1028,
"sea_level" : 1028,
"grnd_level" : 1012,
"humidity" : 80,
"temp_kf" : -1.61},
"weather" : [
{
"id" : 801,
"main" : "Clouds",
"description" : "few clouds",
"icon" : "02n"}
],
"clouds" : {
"all" : 20},
"wind" : {
"speed" : 3.07,
"deg" : 290},
"sys" : {
"pod" : "n"},
"dt_txt" : "2020-01-10 21:00:00"},
{"dt" : 1579100400,"main" : {"temp" : 282.61,"feels_like" : 279.44,"temp_min" : 282.61,"temp_max" : 282.61,"pressure" : 1019,"sea_level" : 1019,"grnd_level" : 1003,"humidity" : 78,"temp_kf" : 0},"weather" : [{"id" : 804,"main" : "Clouds","description" : "overcast clouds","icon" : "04d"
}
],
"clouds" : {"all" : 100},
"wind" : {"speed" : 3.16,"deg" : 226},
"sys" : {"pod" : "d"},
"dt_txt" : "2020-01-15 15:00:00"
}
],
"city" : {
  "id" : 2924625,
  "name" : "Friedrichsdorf",
  "coord" : {"lat" : 50.2496,"lon" : 8.6428 },
  "country" : "DE",
  "timezone" : 3600,
  "sunrise" : 1578640956,
  "sunset" : 1578670940
}
  })==");

      LOGGER_INFO("[[Done.");
#endif

} // addElements()


void Board::start(Element_StartupMode startupMode)
{
  LOGGER_TRACE("start(%d)", startupMode);

  // make elements active that match
  Element *l = _elementList;
  while (l != NULL) {

    if ((!l->active) && (l->startupMode <= startupMode)) {
      // start element when not already active
      LOGGER_TRACE("starting %s...", l->id);
      l->start();
      yield();
    } // if

    l = l->next;
  } // while

  active = true;
  _nextElement = NULL;
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
  _checkNetState();

  if (boardState == BOARDSTATE_RUN) {
    // Most common state.

    if (!startComplete) {
      if (!hasTimeElements) {
        startComplete = true;
      } else {
        // starting time depending elements
        // check if time is valid now -> start all elements with
        time_t current_stamp = getTime();
        if (current_stamp) {
          start(Element_StartupMode::Time);
          startComplete = true;
        } // if
      } // if

      if (startComplete) {
        dispatch(startAction); // dispatched when all elements are active.
      }
    } // if ! startComplete

    // dispatch next action from _actionList if any
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
    if (_nextElement == NULL) {
      _nextElement = _elementList;
    } // if
    if (_nextElement) {
      if (_nextElement->active) {
        _nextElement->loop();
      }
      _nextElement = _nextElement->next;
    } // if

  } else if (boardState == BOARDSTATE_NONE) {
    _newState(BOARDSTATE_LOAD);

  } else if (boardState == BOARDSTATE_LOAD) {
    // load all config files and create+start elements
    addElements();

    // search any time requesting elements
    Element *l = _elementList;
    while (l != nullptr) {
      if (l->startupMode == Element_StartupMode::Time) {
        hasTimeElements = true;
      } // if
      l = l->next;
    } // while

    // setup system wide stuff
    WiFi.hostname(deviceName);
    Wire.begin(I2cSda, I2cScl);

    // LOGGER_INFO("net-status: %d <%s>", WiFi.status(), WiFi.hostname().c_str());

    start(Element_StartupMode::System);
    displayInfo(HOMEDING_GREETING);

    netMode = NetMode_AUTO;

    // detect no configured network situation
    if ((WiFi.SSID().length() == 0) && (strlen(ssid) == 0)) {
      // LOGGER_INFO("ssid empty");
      _newState(BOARDSTATE_STARTCAPTIVE); // start hotspot right now.
    } else if (_resetCount == 2) {
      // LOGGER_INFO("resetcount 2");
      _newState(BOARDSTATE_STARTCAPTIVE); // start hotspot right now.
    } else {
      _newState(BOARDSTATE_CONNECT);
    }

    // wait at least some seconds for offering config mode
    configPhaseEnd = now + nextModeTime;

  } else if (boardState == BOARDSTATE_CONNECT) {
    bool autoCon = WiFi.getAutoConnect();
    LOGGER_TRACE("state: ac=%d mode=%d", autoCon, WiFi.getMode());

    if ((!autoCon) && (netMode == NetMode_AUTO))
      netMode = NetMode_PSK;

    // connect to a same network as before ?
    LOGGER_TRACE("wifi status=(%d)", _wifi_status);
    LOGGER_TRACE("wifi ssid=(%s)", WiFi.SSID().c_str());
    _newState(BOARDSTATE_CONFWAIT);

    if (netMode == NetMode_AUTO) {
      // 1. priority:
      // give autoconnect the chance to do it.
      // works only after a successfull network connection in the past.
      LOGGER_TRACE("NetMode_AUTO");

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
        LOGGER_TRACE("NetMode_PASS SKIP");
        connectPhaseEnd = now;
        return;
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
    connectPhaseEnd = now + maxConnectTime;

  } else if ((boardState == BOARDSTATE_CONFWAIT) || (boardState == BOARDSTATE_WAIT)) {
    // make sysLED blink.
    // short pulses for normal=save mode, long pulses for unsave mode. 
    if (sysLED >= 0) {
      digitalWrite(sysLED, (now % 700) > (savemode ? 100 : 600) ? HIGH : LOW);
    } // if

    // check sysButton
    if ((sysButton >= 0) && (digitalRead(sysButton) == LOW)) {
      // LOGGER_INFO("sysbutton pressed");
      _newState(BOARDSTATE_STARTCAPTIVE);

    } else if (boardState == BOARDSTATE_CONFWAIT) {
      if (now > configPhaseEnd) {
        _newState(BOARDSTATE_WAIT);
      }

    } else {
      if (_wifi_status == WL_CONNECTED) {
        LOGGER_TRACE("connected.");
        WiFi.setAutoReconnect(true);
        WiFi.setAutoConnect(true);
        _newState(BOARDSTATE_GREET);

      } else if ((_wifi_status == WL_NO_SSID_AVAIL) ||
                 (_wifi_status == WL_CONNECT_FAILED) ||
                 (now >= connectPhaseEnd)) {
        netMode -= 1;
        LOGGER_TRACE("wifi status=(%d)", _wifi_status);
        LOGGER_TRACE("next connect method = %d\n", netMode);
        if (netMode) {
          _newState(BOARDSTATE_CONNECT);
        } else {
          LOGGER_INFO("no-net restarting...\n");
          clearResetCount();
          delay(1000);
          ESP.restart();
        }
      } else {
        yield();
      }
    } // if

  } else if (boardState == BOARDSTATE_GREET) {
    clearResetCount();

    displayInfo(WiFi.hostname().c_str(), WiFi.localIP().toString().c_str());
    LOGGER_TRACE("Connected to: %s %s", WiFi.SSID().c_str(), (savemode ? "in savemode" : "unsecured"));
    WiFi.softAPdisconnect(); // after config mode, the AP needs to be closed down.

    if (display) {
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
    dispatch(sysStartAction); // dispatched when network is available

    // start file server for static files in the file system.
    server->serveStatic("/", SPIFFS, "/", "NO-CACHE");

    _newState(BOARDSTATE_RUN);

  } else if (boardState == BOARDSTATE_STARTCAPTIVE) {
    uint8_t mac[6];
    char ssid[64];

    clearResetCount();

    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.macAddress(mac);
    snprintf(ssid, sizeof(ssid), "%s%02X%02X%02X", HOMEDING_GREETING, mac[3], mac[4], mac[5]);

    displayInfo("config..", ssid);

    WiFi.softAP(ssid);
    delay(5);
    // LOGGER_INFO(" AP-IP: %s", WiFi.softAPIP().toString().c_str());

    if (sysLED >= 0) {
      pinMode(sysLED, OUTPUT);
      digitalWrite(sysLED, LOW);
    }

    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(DNS_PORT, "*", apIP);

    server->begin();

    _newState(BOARDSTATE_RUNCAPTIVE);
    _captiveEnd = now + (5 * 60 * 1000);

  } else if (boardState == BOARDSTATE_RUNCAPTIVE) {
    // server.handleClient(); needs to be called in main loop.
    dnsServer.processNextRequest();

    if (now > _captiveEnd)
      reboot(false);
  } // if
} // loop()


Element *Board::findById(String &id)
{
  return (findById(id.c_str()));
}

Element *Board::findById(const char *id)
{
  // LOGGER_TRACE("findById(%s)", id);

  Element *l = _elementList;
  while (l != NULL) {
    if (strcmp(l->id, id) == 0) {
      // LOGGER_TRACE(" found:%s", l->id);
      break; // while
    } // if
    l = l->next;
  } // while
  return (l);
} // findById


// send a event out to the defined target.
void Board::_dispatchSingle(String evt)
{
  // LOGGER_TRACE("dispatch %s", evt.c_str());

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
      // also show action in log when target has trace loglevel
      if ((Logger::logger_level < LOGGER_LEVEL_TRACE) && (target->loglevel >= LOGGER_LEVEL_TRACE))
        Logger::LoggerPrint("sys", LOGGER_LEVEL_TRACE, "dispatch (%s)", evt.c_str());

      if (!ret)
        LOGGER_ERR("Event '%s' was not handled", evt.c_str());
    }
  }
} // _dispatchSingle()


/**
 * @brief Save an action to the _actionList.
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
 * @brief Save an action to the _actionList.
 */
void Board::dispatch(String &action, const char *value)
{
  if (action.length() > 0)
    dispatch(action.c_str(), value);
} // dispatch


/**
 * @brief Save an action to the _actionList.
 */
void Board::dispatch(String &action, String &value)
{
  if (action.length() > 0)
    dispatch(action.c_str(), value.c_str());
} // dispatch


/**
 * @brief Save an action to the _actionList.
 */
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


/**
 * @brief Save an action to the _actionList using a item part of a value.
 */
void Board::dispatchItem(String &action, String &values, int item)
{
  if (action && values) {
    String v = Element::Element::getItemValue(values, item);
    if (v) {
      dispatch(action.c_str(), v.c_str());
    } // if
  } // if
} // dispatchItem


void Board::getState(String &out, const String &path)
{
  // LOGGER_TRACE("getState(%s)", path.c_str());
  String ret = "{";
  const char *cPath = path.c_str();

  Element *l = _elementList;
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
  if (e) {
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
Element *Board::getElement(const char *elementType)
{
  // LOGGER_TRACE("getElement(%s)", elementType);

  String tn = elementType;
  tn.concat(ELEM_ID_SEPARATOR);
  int tnLength = tn.length();

  Element *l = _elementList;
  while (l != NULL) {
    if (String(l->id).substring(0, tnLength).equalsIgnoreCase(tn)) {
      break; // while
    } // if
    l = l->next;
  } // while
  // LOGGER_TRACE("found: %d", l);
  return (l);
} // getElement()


Element *Board::getElement(const char *elementType, const char *elementName)
{
  // LOGGER_TRACE("getElement(%s/%s)", elementType, elementName);

  String tn = elementType;
  tn.concat('/');
  tn.concat(elementName);

  Element *l = _elementList;
  while (l != NULL) {
    if (String(l->id).equalsIgnoreCase(tn)) {
      break; // while
    } // if
    l = l->next;
  } // while
  // LOGGER_TRACE("found: %d", l);
  return (l);

} // getElement()


void Board::reboot(bool wipe)
{
  LOGGER_INFO("reboot...");
  if (wipe)
    WiFi.disconnect(true);
  delay(1000);
  ESP.reset();
};


void Board::displayInfo(const char *text1, const char *text2)
{
  LOGGER_INFO("%s %s", text1, text2 ? text2 : "");
  if (display) {
    display->clear();
    display->drawText(0, 0, 0, text1);
    if (text2) {
      display->drawText(0, display->getLineHeight(), 0, text2);
    }
    display->flush();
  } // if
}


/**
 * @brief Add another element to the board into the list of created elements.
 */
void Board::_add(const char *id, Element *e)
{
  // LOGGER_TRACE("_add(%s)", id);

  strcpy(e->id, id);
  Element::_strlower(e->id);
  e->next = NULL;
  Element *l = _elementList;

  // append to end of list.
  if (l == NULL) {
    // first Element.
    _elementList = e;
  } else {
    // search last Element.
    while (l->next != NULL)
      l = l->next;
    l->next = e;
  } // if
  e->init(this);
} // _add()


// End
