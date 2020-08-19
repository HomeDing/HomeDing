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
#include <ESP8266mDNS.h>

extern "C" {
#include <user_interface.h> // https://github.com/esp8266/Arduino actually tools/sdk/include
}

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


// ===== detect request for network config and non-safemode using 2 resets in less than 2 sec.
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

bool strStartsWithIgnore(const char *s, const char *prefix)
{
  bool ret = true; // until we find a difference
  if (s && prefix) {
    while (ret && *s && *prefix) {
      if (tolower(*s) != tolower(*prefix))
        ret = false;
      s++;
      prefix++;
    } // while
    if (*prefix)
      ret = false;
  } else {
    ret = false;
  }
  return (ret);
} // strStartsWithIgnore

/**
 * @brief Initialize a blank board.
 */
void Board::init(ESP8266WebServer *serv)
{
  server = serv;

  // board parameters overwritten by device element
  sysLED = -1; // configured by device-element
  sysButton = -1; // configured by device-element
  boardState = BOARDSTATE_NONE;
  homepage = "/index.htm";
  deepSleepStart = 0; // no deep sleep to be started
  deepSleepBlock = false; // no deep sleep is blocked
  deepSleepTime = 60; // one minute

  _cntDeepSleep = 0;

  rst_info *ri = ESP.getResetInfoPtr();
  isWakeupStart = (ri->reason == REASON_DEEP_SLEEP_AWAKE);
  if (isWakeupStart) {
    LOGGER_INFO("Reset from Deep Sleep mode.");
  }

  WiFi.begin();
  deviceName = WiFi.hostname(); // use mac based default device name
  deviceName.replace("_", ""); // Underline in hostname is not conformant, see
      // https://tools.ietf.org/html/rfc1123 952
} // init()


/** Return true when board is runing in captive mode. */
bool Board::isCaptiveMode()
{
  return ((boardState == BOARDSTATE_STARTCAPTIVE) || (boardState == BOARDSTATE_RUNCAPTIVE));
} // isCaptiveMode()


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
void Board::_addAllElements()
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
          // create new element
          LOGGER_TRACE("new %s", path);
          // extract type name
          char typeName[32];

          char *p = path;
          char *t = typeName;
          while (*p && *p != MICROJSON_PATH_SEPARATOR) {
            *t++ = *p++;
          } // while
          *t = '\0';

          // typeName starts with "web" ?
          if (strStartsWithIgnore(typeName, "web")) {
            // don't try to create web elements
            _lastElem = nullptr;
          } else {
            _lastElem = ElementRegistry::createElement(typeName);
            if (!_lastElem) {
              LOGGER_ERR("Cannot create Element type %s", typeName);
            } else {
              // add to the list of elements
              _addElement(path, _lastElem);
            }
          } // if

        } else if ((level > 2) && (_lastElem)) {
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

  if (!_elementList) {
    // no elemenmt defined so allow configuration in any case.
    isSafeMode = false;
  }

  delete mj;
} // _addAllElements()


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
  LOGGER_TRACE("Set state=%d", newState);
  boardState = newState;
}

// loop next element, only one at a time!
void Board::loop()
{
  unsigned long now = millis();
  _checkNetState();

  if (boardState == BOARDSTATE_RUN) {
    // Most common state first.
    if (!isWakeupStart)
      MDNS.update();

    if (!startComplete) {
      if (!hasTimeElements) {
        startComplete = true;
      } else {

        // starting time depending elements
        // check if time is valid now -> start all elements with
        time_t ct = time(nullptr);
        if (ct) {
          start(Element_StartupMode::Time);
          startComplete = true;
        } // if
      } // if

      if (startComplete) {
        dispatch(startAction); // dispatched when all elements are active.
        LOGGER_INFO("Connected to %s %s", WiFi.SSID().c_str(), (isSafeMode ? "safemode" : "unsafe"));
      }
    } // if ! startComplete

    // dispatch next action from _actionList if any
    if (_actionList.length() > 0) {
      _cntDeepSleep = 0;
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

    if ((!deepSleepBlock) && (deepSleepStart > 0)) {
      _cntDeepSleep++;

      // deep sleep specified time.
      if ((now > deepSleepStart) && (_cntDeepSleep > _addedElements + 4)) {
        // all elements now had the chance to create and dispatch an event.
        LOGGER_INFO("sleep %d...", deepSleepTime);
        Serial.flush();
        ESP.deepSleep(deepSleepTime * 1000 * 1000);
        delay(1);
        _newState(BOARDSTATE_SLEEP);
      }
    } // if

  } else if (boardState == BOARDSTATE_NONE) {
    _newState(BOARDSTATE_LOAD);

  } else if (boardState == BOARDSTATE_LOAD) {
    // load all config files and create+start elements
    _addAllElements();

    _resetCount = getResetCount();

    if (_resetCount > 0) {
      // enforce un-safemode on double reset
      LOGGER_TRACE("RESET # %d", _resetCount);
      isSafeMode = false;
    } // if

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

    start(Element_StartupMode::System);
    displayInfo(HOMEDING_GREETING);

    netMode = NetMode_AUTO;

    // Enable sysLED for blinking while waiting for network or config mode
    if (sysLED >= 0) {
      pinMode(sysLED, OUTPUT);
      digitalWrite(sysLED, HIGH);
    }

    // detect no configured network situation
    if (((WiFi.SSID().length() == 0) && (strlen(ssid) == 0)) || (_resetCount == 2)) {
      _newState(BOARDSTATE_STARTCAPTIVE); // start hotspot right now.
    } else {
      _newState(BOARDSTATE_CONNECT);
    }

    // wait at least some seconds for offering config mode
    configPhaseEnd = now + minConfigTime;


  } else if (boardState == BOARDSTATE_CONNECT) {
    bool autoCon = WiFi.getAutoConnect();
    LOGGER_TRACE("state: ac=%d mode=%d", autoCon, WiFi.getMode());

    if ((!autoCon) && (netMode == NetMode_AUTO))
      netMode = NetMode_PSK;

    // connect to a same network as before ?
    LOGGER_TRACE("wifi status=(%d)", _wifi_status);
    LOGGER_TRACE("wifi ssid=(%s)", WiFi.SSID().c_str());
    _newState(BOARDSTATE_WAITNET);

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

    // Enable sysButton for entering config mode
    if (sysButton >= 0) {
      pinMode(sysButton, INPUT_PULLUP);
    }

    // wait max. 4 seconds for connecting to the network
    connectPhaseEnd = now + maxNetConnextTime;
    // LOGGER_TRACE("  set phase: %ld %ld %d", now, connectPhaseEnd, maxNetConnextTime);


  } else if ((boardState == BOARDSTATE_WAITNET) || (boardState == BOARDSTATE_WAIT)) {
    // make sysLED blink.
    // short pulses for normal=safemode, long pulses for unsafemode.
    if (sysLED >= 0) {
      digitalWrite(sysLED, (now % 700) > (isSafeMode ? 100 : 600) ? HIGH : LOW);
    } // if

    // check sysButton
    if ((sysButton >= 0) && (digitalRead(sysButton) == LOW)) {
      // LOGGER_INFO("sysbutton pressed");
      _newState(BOARDSTATE_STARTCAPTIVE);
    }

    if (boardState == BOARDSTATE_WAITNET) {
      if (_wifi_status == WL_CONNECTED) {
        LOGGER_TRACE("connected.");
        WiFi.setAutoReconnect(true);
        WiFi.setAutoConnect(true);
        _newState(BOARDSTATE_WAIT);
      } // if

      if ((_wifi_status == WL_NO_SSID_AVAIL) ||
          (_wifi_status == WL_CONNECT_FAILED) ||
          (now > connectPhaseEnd)) {

        if (now > connectPhaseEnd) {
          LOGGER_TRACE("timed out.");
        } else {
          LOGGER_TRACE("wifi status=(%d)", _wifi_status);
        } // if

        netMode -= 1;
        // LOGGER_TRACE("next connect method = %d\n", netMode);
        if (netMode) {
          _newState(BOARDSTATE_CONNECT); // try next mode
        } else {
          LOGGER_INFO("no-net restarting...\n");
          clearResetCount();
          delay(500);
          ESP.restart();
        } // if
      } // if
    } // if

    if (boardState == BOARDSTATE_WAIT) {
      if (isWakeupStart || (now >= configPhaseEnd)) {
        _newState(BOARDSTATE_GREET);
      }
    } // if
    yield();


  } else if (boardState == BOARDSTATE_GREET) {
    clearResetCount();

    displayInfo(WiFi.hostname().c_str(), WiFi.localIP().toString().c_str());
    LOGGER_TRACE("Connected to %s %s", WiFi.SSID().c_str(), (isSafeMode ? "safemode" : "unsafe"));
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
    server->enableCORS(true);
    start(Element_StartupMode::Network);
    dispatch(sysStartAction); // dispatched when network is available

    // ===== initialize network dependant services

    // start file server for static files in the file system.
    server->serveStatic("/", SPIFFS, "/", "NO-CACHE");

    // start mDNS service discovery for "_homeding._tcp"
    // but not when using deep sleep mode
    if (!isWakeupStart && (mDNS_sd)) {
      MDNS.begin(deviceName.c_str());
      MDNSResponder::hMDNSService serv = MDNS.addService(0, "homeding", "tcp", 80);
      MDNS.addServiceTxt(serv, "path", homepage.c_str());
    } // if

    _newState(BOARDSTATE_RUN);
  } else if (boardState == BOARDSTATE_SLEEP) {
    // just wait.
    Serial.write('*');
    Serial.flush();
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

    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(DNS_PORT, "*", apIP);

    server->begin();

    _newState(BOARDSTATE_RUNCAPTIVE);
    _captiveEnd = now + (5 * 60 * 1000);
  } else if (boardState == BOARDSTATE_RUNCAPTIVE) {
    // server.handleClient(); needs to be called in main loop.
    dnsServer.processNextRequest();

    // make sysLED blink 3 sec with a short flash.
    if (sysLED >= 0) {
      digitalWrite(sysLED, ((now % 3000) > 120) ? HIGH : LOW);
    } // if

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


/**
 * do not start sleep mode because element is active.
 */
void Board::deferSleepMode()
{
  // reset the counter to ensure looping all active elements
  _cntDeepSleep = 0;
} // deferSleepMode()


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
  time_t current_stamp = time(nullptr);
  if (current_stamp <= MIN_VALID_TIME) {
    current_stamp = 0;
  } // if
  return (current_stamp);
} // getTime()


// return the seconds of today in localtime.
time_t Board::getTimeOfDay()
{
  time_t ct = time(nullptr);
  tm lt;
  if (ct) {
    localtime_r(&ct, &lt);
    return ((lt.tm_hour * 60 * 60) + (lt.tm_min * 60) + lt.tm_sec);
  } else {
    return (0);
  }
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
  LOGGER_TRACE("%s %s", text1, text2 ? text2 : "");
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
void Board::_addElement(const char *id, Element *e)
{
  // LOGGER_TRACE("_add(%s)", id);
  _addedElements++;

  strcpy(e->id, id);
  Element::_strlower(e->id);
  Element *l = _elementList;

  // append to end of list.
  if (!l) {
    // first Element.
    _elementList = e;
  } else {
    // search last Element.
    while (l->next)
      l = l->next;
    // append.
    l->next = e;
  } // if
  e->next = nullptr;
  e->init(this);
} // _addElement()


// End
