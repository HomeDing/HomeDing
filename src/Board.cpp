/**
 * @file Board.cpp
 * @author Matthias Hertel (https://www.mathertel.de)
 *
 * @brief Implementation of the Board class for the HomeDing Library
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino.
 *
 * Changelog: see Board.h
 */

// WiFi: 1,7 tracing:
//   (mode) 1: WIFI_STA, 2: WIFI_AP, 3: Both
//   (status) 1: no SSID, 3: CONNECTED, 4: FAILED, 6: PASSWORD WRONG, 7: DISCONNECTED

#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#if defined(ESP8266)
#include <ESP8266mDNS.h>
extern "C" {
#include <user_interface.h>  // https://github.com/esp8266/Arduino actually tools/sdk/include
}
#include "sntp.h"

#elif defined(ESP32)
#include <ESPmDNS.h>
#endif

#include <ElementRegistry.h>

#include "MicroJsonParser.h"

#include <DNSServer.h>

// use TRACE for compiling with detailed TRACE output.
#define TRACE(...)  // LOGGER_TRACE(__VA_ARGS__)

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

static const char respond404[] PROGMEM =
  "<html><head><title>File not found</title></head><body>File not found</body></html>";

/**
 * @brief Initialize a blank board.
 */
void Board::init(WebServer *serv, FILESYSTEM *fs, const char *buildName) {
  WiFi.persistent(true);

  LOGGER_INFO("Device %s starting...", buildName);

  server = serv;
  fileSystem = fs;
  build = buildName;

  bool mounted = fileSystem->begin();
  if (!mounted) {
    LOGGER_INFO("formatting...");
    fileSystem->format();
  }

  Logger::init(fileSystem);

  // board parameters configured / overwritten by device element
  sysLED = -1;
  sysButton = -1;
  homepage = "/index.htm";
  cacheHeader = "no-cache";

  _newState(BOARDSTATE::NONE);
  _deepSleepStart = 0;      // no deep sleep to be started
  _deepSleepBlock = false;  // no deep sleep is blocked
  _deepSleepTime = 60;      // one minute

  _cntDeepSleep = 0;

#if defined(ESP8266)
  rst_info *ri = ESP.getResetInfoPtr();
  _isWakeupStart = (ri->reason == REASON_DEEP_SLEEP_AWAKE);
  if (_isWakeupStart) {
    LOGGER_INFO("Reset from Deep Sleep mode.");
  }
  enableWiFiAtBootTime();

#elif defined(ESP32)
  _isWakeupStart = false;  // TODO:ESP32 ???
#endif

  deviceName = WiFi.getHostname();  // use mac based default device name
  TRACE("dn1: %s", deviceName.c_str());
  deviceName.replace("_", "");  // Underline in hostname is not conformant, see
  // https://tools.ietf.org/html/rfc1123 952
  TRACE("dn2: %s", deviceName.c_str());
  hd_yield();
}  // init()


void Board::add(const char *id, Element *e) {
  // LOGGER_TRACE("add(%s)", id);
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
  }  // if
  e->next = nullptr;
  e->init(this);
}


/**
 * @brief Check the mode the device in running.
 * @return true when board is runing in captive mode.
 * @return false when board is runing in normal mode.
 */
bool Board::isCaptiveMode() {
  return ((boardState == BOARDSTATE::STARTCAPTIVE) || (boardState == BOARDSTATE::RUNCAPTIVE));
}  // isCaptiveMode()


/**
 * @brief
 */
void Board::_checkNetState() {
  hd_yield();
  wl_status_t newState = WiFi.status();
  if (newState != _wifi_status) {
    TRACE("new netstate: %d", newState);
    _wifi_status = newState;
  }
}


// Remark: some settings on the board class can be modified using the device element.
// see deviceElement.cpp


/**
 * @brief Add and config the Elements defined in the config files.
 */
void Board::_addAllElements() {
  // TRACE("addElements()");
  Element *_lastElem = NULL;  // last created Element

  MicroJson *mj = new MicroJson(
    [this, &_lastElem](int level, char *path, char *value) {
      // TRACE("callback %d %s =%s", level, path, value ? value : "-");
      _checkNetState();

      if (level == 1) {

      } else if (level == 2) {
        // create new element
        TRACE("new %s", path);
        // extract type name
        char typeName[32];

        char *p = path;
        char *t = typeName;
        while (*p && *p != MICROJSON_PATH_SEPARATOR) {
          *t++ = *p++;
        }  // while
        *t = '\0';

        // typeName starts with "web" ?
        if (Element::_stristartswith(typeName, "web")) {
          // don't try to create web elements
          _lastElem = nullptr;
        } else {
          _lastElem = ElementRegistry::createElement(typeName);
          if (!_lastElem) {
            LOGGER_ERR("Cannot create Element type %s", typeName);
          } else {
            // add to the list of elements
            add(path, _lastElem);
          }
        }  // if

      } else if ((level > 2) && (_lastElem) && (value)) {
        // Search 2. slash as starting point for name to include 3. level
        char *name = strchr(path, MICROJSON_PATH_SEPARATOR) + 1;
        name = strchr(name, MICROJSON_PATH_SEPARATOR) + 1;
        TRACE(" (%s) %s=%s", path, name, value ? value : "-");
        // add a parameter to the last Element
        _lastElem->set(name, value);
      }  // if
    });

  if (mj) {
    // config the thing to the local network
    mj->parseFile(fileSystem, ENV_FILENAME);
    _checkNetState();

    // config the Elements of the device
    mj->parseFile(fileSystem, CONF_FILENAME);
    _checkNetState();
  }  // if

  delete mj;
}  // _addAllElements()


void Board::start(Element_StartupMode startupMode) {
  // TRACE("start(%d)", startupMode);

  // make elements active that match
  Element *l = _elementList;
  while (l != NULL) {

    if ((!l->active) && (l->startupMode <= startupMode)) {
      // start element when not already active
      TRACE("starting %s...", l->id);
      l->setup();  // one-time initialization
      l->start();  // start...
      hd_yield();
    }  // if

    l = l->next;
  }  // while

  active = true;
  _nextElement = NULL;
}  // start()


// switch to a new state
void Board::_newState(enum BOARDSTATE newState) {
  hd_yield();
  // TRACE("WiFi: %d,%d", WiFi.getMode(), WiFi.status());
  TRACE("State=%d", newState);
  boardState = newState;
}

// loop next element, only one at a time!
void Board::loop() {
  unsigned long now = millis();
  _checkNetState();

  if (boardState == BOARDSTATE::RUN) {
    // Most common state first.
#if defined(ESP8266)
    if (!_isWakeupStart) {
      MDNS.update();
    }
#endif

    if (!startComplete) {
      if (!hasTimeElements) {
        startComplete = true;
      } else {

        // starting time depending elements
        // check if time is valid now -> start all elements with
        if (time(nullptr)) {
          start(Element_StartupMode::Time);
          startComplete = true;
        }  // if
      }    // if

      if (startComplete) {
        dispatch(startAction);  // dispatched when all elements are active.
      }
    }  // if ! startComplete

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
      }  // if
      dispatchAction(_lastAction);
      return;
    }  // if

    // give some time to next active element
    if (_nextElement == NULL) {
      _nextElement = _elementList;
    }  // if
    if (_nextElement) {
      if (_nextElement->active) {
        TRACE_START;
        _nextElement->loop();
        TRACE_END;
        TRACE_TIMEPRINT("loop", _nextElement->id, 5);
      }
      _nextElement = _nextElement->next;
    }  // if

    if ((!_deepSleepBlock) && (_deepSleepStart > 0)) {
      _cntDeepSleep++;

      // deep sleep specified time.
      if ((now > _deepSleepStart) && (_cntDeepSleep > _addedElements + 4)) {
        // all elements now had the chance to create and dispatch an event.
        LOGGER_INFO("sleep %d...", _deepSleepTime);
        Serial.flush();
        ESP.deepSleep(_deepSleepTime * 1000 * 1000);
        _newState(BOARDSTATE::SLEEP);
      }
    }  // if

  } else if (boardState == BOARDSTATE::NONE) {
    TRACE("AutoConnect=%d", WiFi.getAutoConnect());
    _newState(BOARDSTATE::LOAD);

  } else if (boardState == BOARDSTATE::LOAD) {
    // load all config files and create+start elements
    _addAllElements();
    if (!_elementList) {
      Logger::logger_level = LOGGER_LEVEL_TRACE;
      // no element defined, so allow configuration in any case.
      isSafeMode = false;
      // start up ota
      Element *e = ElementRegistry::createElement("ota");
      add("ota/0", e);
    }

    if (state) {
      state->load();
    }

    _resetCount = RTCVariables::getResetCounter();

    if (_resetCount > 0) {
      // enforce un-safemode on double reset
      LOGGER_TRACE("Reset #%d", _resetCount);
      isSafeMode = false;
    }  // if
    _resetCount = RTCVariables::setResetCounter(_resetCount + 1);

    // search any time requesting elements
    Element *l = _elementList;
    while (l != nullptr) {
      if (l->startupMode == Element_StartupMode::Time) {
        hasTimeElements = true;
      }  // if
      l = l->next;
    }  // while

    // setup system wide stuff
    WiFi.setHostname(deviceName.c_str());
    Wire.begin(I2cSda, I2cScl);

    start(Element_StartupMode::System);
    displayInfo(HOMEDING_GREETING);

    if (WiFi.SSID().length() > 0) {
      netMode = NetMode_AUTO;
    } else {
      netMode = NetMode_PASS;
    }

    // Enable sysLED for blinking while waiting for network or config mode
    if (sysLED >= 0) {
      pinMode(sysLED, OUTPUT);
      digitalWrite(sysLED, HIGH);
    }

    // TRACE("WiFi.SSID=%s", WiFi.SSID().c_str());
    // TRACE("resetCount=%d", _resetCount);

    // detect no configured network situation
    if (((WiFi.SSID().length() == 0) && (strnlen(ssid, 2) == 0)) || (_resetCount == 2)) {
      _newState(BOARDSTATE::STARTCAPTIVE);  // start hotspot right now.
    } else {
      _newState(BOARDSTATE::CONNECT);
    }

    // wait at least some seconds for offering config mode
    configPhaseEnd = now + minConfigTime;


  } else if (boardState == BOARDSTATE::CONNECT) {
    // TRACE("autoconnect=%d", WiFi.getAutoConnect());

    _newState(BOARDSTATE::WAITNET);

    if (netMode == NetMode_AUTO) {
      // 1. priority:
      // give autoconnect the chance to do it.
      // works only after a successfull network connection in the past.
      LOGGER_TRACE("NetMode_AUTO: %s", WiFi.SSID().c_str());

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
      TRACE("NetMode_PASS: %s", ssid);

      if (!*ssid) {
        TRACE("SKIP");
        connectPhaseEnd = 0;
        return;
      } else {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, passPhrase);
      }
    }  // if

    // Enable sysButton for entering config mode
    if (sysButton >= 0) {
      pinMode(sysButton, INPUT_PULLUP);
    }

    // wait some(12) seconds for connecting to the network
    connectPhaseEnd = now + maxNetConnextTime;

  } else if ((boardState == BOARDSTATE::WAITNET) || (boardState == BOARDSTATE::WAIT)) {
    // make sysLED blink.
    // short pulses for normal=safemode, long pulses for unsafemode.
    if (sysLED >= 0) {
      digitalWrite(sysLED, (now % 700) > (isSafeMode ? 100 : 600) ? HIGH : LOW);
    }  // if

    // check sysButton
    if ((sysButton >= 0) && (digitalRead(sysButton) == LOW)) {
      // TRACE("sysbutton pressed");
      _newState(BOARDSTATE::STARTCAPTIVE);
    }

    if (boardState == BOARDSTATE::WAITNET) {
      if (_wifi_status == WL_CONNECTED) {
        LOGGER_TRACE("connected.");
        WiFi.setAutoReconnect(true);
        WiFi.setAutoConnect(true);
        _newState(BOARDSTATE::WAIT);
      }  // if

      if ((_wifi_status == WL_NO_SSID_AVAIL) || (_wifi_status == WL_CONNECT_FAILED) || (now > connectPhaseEnd)) {

        if (!connectPhaseEnd) {
          // no LOGGER_TRACE;
        } else if (now > connectPhaseEnd) {
          LOGGER_TRACE("timed out.");
        } else {
          LOGGER_TRACE("wifi status=(%d)", _wifi_status);
        }  // if

        netMode -= 1;
        // LOGGER_TRACE("next connect method = %d\n", netMode);
        if (netMode) {
          _newState(BOARDSTATE::CONNECT);  // try next mode
        } else {
          LOGGER_INFO("no-net");
          _resetCount = RTCVariables::setResetCounter(0);

          delay(500);
          ESP.restart();
        }  // if
      }    // if
    }      // if

    if (boardState == BOARDSTATE::WAIT) {
      if (_isWakeupStart || (now >= configPhaseEnd)) {
        _newState(BOARDSTATE::GREET);
      }
    }  // if
    hd_yield();


  } else if (boardState == BOARDSTATE::GREET) {
    _resetCount = RTCVariables::setResetCounter(0);

    const char *name = WiFi.getHostname();

    displayInfo(name, WiFi.localIP().toString().c_str());
    LOGGER_JUSTINFO("connected to %s (%s mode)",
                    WiFi.SSID().c_str(),
                    (isSafeMode ? "safe" : "unsafe"));

    LOGGER_JUSTINFO("start http://%s/", name);

    if (WiFi.getMode() == WIFI_AP_STA) {
      WiFi.mode(WIFI_STA);  // after config mode, the AP needs to be closed down and Station Mode can start.
    }

    if (display) {
      delay(1600);
      display->clear();
      display->flush();
    }  // if

    // release sysLED
    if (sysLED >= 0) {
      digitalWrite(sysLED, HIGH);
      pinMode(sysLED, INPUT);
    }

    server->begin();
    server->enableCORS(true);

    randomSeed(millis());         // millis varies on every start, good enough
    filesVersion = random(8000);  // will incremented on every file upload by file server

    if (cacheHeader == "etag") {
#if defined(HOMEDING_SUPPORT_ETAG)
      // enable eTags in results for static files
      // by setting "cache": "etag" inc env.json on the device element

      // This is a fast custom eTag generator. It returns a current number that gets incremented when any file is updated.
      server->enableETag(true, [this](FILESYSTEM &, const String &path) -> String {
        String eTag;
        if (!path.endsWith(".txt")) {
          // txt files contain logs that must not be cached.
          // eTag = esp8266webserver::calcETag(fs, path);
          // File f = fs.open(path, "r");
          // eTag = f.getLastWrite()
          // f.close();
          // use current counter
          eTag = String(filesVersion, 16);  // f.getLastWrite()
        }
        return (eTag);
      });
#endif
      cacheHeader = "";  // do not pass this cache header
    }

    start(Element_StartupMode::Network);
    dispatch(sysStartAction);  // dispatched when network is available

    // ===== initialize network dependant services

    // start file server for static files in the file system.
    server->serveStatic("/", *fileSystem, "/", cacheHeader.c_str());

    server->onNotFound([this]() {
      TRACE("notFound: %s", server->uri().c_str());
      server->send(404, "text/html", FPSTR(respond404));
    });


    // start mDNS service discovery for "_homeding._tcp"
    // but not when using deep sleep mode
    if (!_isWakeupStart && (mDNS_sd)) {

#if defined(ESP8266)
      MDNS.begin(deviceName.c_str());
      // include the data required for the portal implementation: Overview of existing devices
      MDNSResponder::hMDNSService serv;

      serv = MDNS.addService(0, "homeding", "tcp", 80);
      MDNS.addServiceTxt(serv, "path", homepage.c_str());
      MDNS.addServiceTxt(serv, "title", title.c_str());
      MDNS.addServiceTxt(serv, "room", room.c_str());

#elif defined(ESP32x)
      // using https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/mdns.html
      // https://github.com/espressif/esp-idf/blob/master/examples/protocols/mdns/main/mdns_example_main.c

      LOGGER_JUSTINFO("a0");
      // initialize mDNS service
      esp_err_t err = mdns_init();
      if (err) {
        LOGGER_JUSTINFO("MDNS Init failed: %d\n", err);
        return;
      }

      // set hostname
      mdns_hostname_set(deviceName.c_str());
      // set default instance
      mdns_instance_name_set("HomeDing on ESP32");

      LOGGER_JUSTINFO("b0");
      mdns_service_add(NULL, "_homeding", "_tcp", 80, NULL, 0);

      LOGGER_JUSTINFO("a");
      mdns_txt_item_t txtData[3] = {
        { "path", homepage.c_str() },
        { "title", title.c_str() },
        { "room", room.c_str() }
      };
      LOGGER_JUSTINFO("b");

      // if (mdns_service_txt_set("_homeding", "_tcp", txtData, 3)) {
      //   LOGGER_ERR("Failed adding HomeDing txt");
      // }
      LOGGER_JUSTINFO("c");

#elif defined(ESP32)
      LOGGER_JUSTINFO("a1===");
      // MDNS.begin(deviceName.c_str());
      // MDNS.addService("_homeding", "_tcp", 80);
      // MDNS.addServiceTxt("_homeding", "_tcp", "path", homepage.c_str());
      // MDNS.addServiceTxt("_homeding", "_tcp", "title", title.c_str());
      // MDNS.addServiceTxt("_homeding", "_tcp", "path", "/index.htm");
      // MDNS.addServiceTxt("_homeding", "_tcp", "title", "try me");
      // MDNS.addServiceTxt("_homeding", "_tcp", "room", room.c_str());
      LOGGER_JUSTINFO("b1");
#endif
    }  // if

    _newState(BOARDSTATE::RUN);
  } else if (boardState == BOARDSTATE::SLEEP) {
    // just wait.

  } else if (boardState == BOARDSTATE::STARTCAPTIVE) {
    uint8_t mac[6];
    char ssid[64];

    _resetCount = RTCVariables::setResetCounter(0);

    WiFi.macAddress(mac);
    snprintf(ssid, sizeof(ssid), "%s%02X%02X%02X", HOMEDING_GREETING, mac[3], mac[4], mac[5]);

    displayInfo("config..", ssid);

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP(ssid);
    hd_yield();
    LOGGER_INFO(" AP-IP: %s", WiFi.softAPIP().toString().c_str());

    dnsServer.setTTL(300);
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
    dnsServer.start(DNS_PORT, "*", apIP);

    server->begin();

    _newState(BOARDSTATE::RUNCAPTIVE);
    _captiveEnd = now + (5 * 60 * 1000);

  } else if (boardState == BOARDSTATE::RUNCAPTIVE) {
    // server.handleClient(); needs to be called in main loop.
    dnsServer.processNextRequest();

    // make sysLED blink 3 sec with a short flash.
    if (sysLED >= 0) {
      digitalWrite(sysLED, ((now % 3000) > 120) ? HIGH : LOW);
    }  // if

    if (now > _captiveEnd)
      reboot(false);
  }  // if
}  // loop()


// ===== set board behavior

// start deep sleep mode when idle.
void Board::setSleepTime(unsigned long secs) {
  TRACE("setSleepTime(%d)", secs);
  _deepSleepTime = secs;
}  // setSleepTime()


// start deep sleep mode when idle.
void Board::startSleep() {
  TRACE("startSleep");
  _deepSleepStart = millis();
  if (!_isWakeupStart) {
    // give a minute time to block deep sleep mode
    _deepSleepStart += 60 * 1000;
  }
}  // startSleep()


// block any deep sleep until next reset.
void Board::cancelSleep() {
  TRACE("cancelSleep");
  _deepSleepBlock = true;
}  // cancelSleep()


Element *Board::findById(String &id) {
  return (findById(id.c_str()));
}

Element *Board::findById(const char *id) {
  // LOGGER_TRACE("findById(%s)", id);

  Element *l = _elementList;
  while (l != NULL) {
    if (strcmp(l->id, id) == 0) {
      // LOGGER_TRACE(" found:%s", l->id);
      break;  // while
    }         // if
    l = l->next;
  }  // while
  return (l);
}  // findById


// ===== queue / process / dispatch actions =====

bool Board::queueIsEmpty() {
  return (_actionList.length() == 0);
}


/** Queue an action for later dispatching. */
void Board::_queueAction(const String &action, const String &v) {
  String tmp = action;
  tmp.replace("$v", v);

  if (_actionList.length() > 0)
    _actionList.concat(ACTION_SEPARATOR);
  _actionList.concat(tmp);
}  // _queueAction


// send a event out to the defined target.
void Board::dispatchAction(String action) {
  TRACE("dispatch %s", action.c_str());
  // TRACE_START;

  int pos1 = action.indexOf(ELEM_PARAMETER);
  int pos2 = action.indexOf(ELEM_VALUE);

  if (pos1 <= 0) {
    LOGGER_ERR("No action given: %s", action.c_str());

  } else {
    String targetName, name, value;

    targetName = action.substring(0, pos1);
    targetName.toLowerCase();

    Element *target = findById(targetName);

    if (!target) {
      LOGGER_ERR("dispatch target %s not found", targetName.c_str());

    } else {
      if (pos2 > 0) {
        name = action.substring(pos1 + 1, pos2);
        value = action.substring(pos2 + 1);
      } else {
        name = action.substring(pos1 + 1);
        value = "";
      }
      bool ret = target->set(name.c_str(), value.c_str());
      // also show action in log when target has trace loglevel
      if ((Logger::logger_level < LOGGER_LEVEL_TRACE) && (target->loglevel >= LOGGER_LEVEL_TRACE))
        Logger::LoggerPrint("sys", LOGGER_LEVEL_TRACE, "dispatch (%s)", action.c_str());

      if (!ret)
        LOGGER_ERR("Event '%s' was not handled", action.c_str());
    }
  }
  // TRACE_END;
  // TRACE_TIMEPRINT("used time:", "", 25);
}  // dispatchAction()


/**
 * @brief Save an action to the _actionList.
 */
void Board::dispatch(String &action, int value) {
  if (!action.isEmpty())
    _queueAction(action, String(value));
}  // dispatch


/**
 * @brief Save an action to the _actionList.
 */
void Board::dispatch(String &action, const char *value) {
  if (!action.isEmpty())
    _queueAction(action, String(value));
}  // dispatch


/**
 * @brief Save an action to the _actionList.
 */
void Board::dispatch(const String &action, const String &value) {
  if (!action.isEmpty())
    _queueAction(action, value);
}  // dispatch


/**
 * @brief Save an action to the _actionList using a item part of a value.
 */
void Board::dispatchItem(String &action, String &values, int n) {
  if (action && values) {
    String v = Element::getItemValue(values, n);
    if (v) _queueAction(action, v);
  }  // if
}  // dispatchItem


// ===== low power / sleep mode =====

/**
 * do not start sleep mode because element is active.
 */
void Board::deferSleepMode() {
  // reset the counter to ensure looping all active elements
  _cntDeepSleep = 0;
}  // deferSleepMode()


void Board::getState(String &out, const String &path) {
  // TRACE("getState(%s)", path.c_str());
  String ret = "{";
  const char *cPath = path.c_str();

  Element *l = _elementList;
  while (l != NULL) {
    // LOGGER_TRACE("  ->%s", l->id);
    if ((cPath[0] == '\0') || (strcmp(l->id, cPath) == 0)) {
      ret += '\"';
      ret += l->id;
      ret += "\":{";
      l->pushState([&ret](const char *name, const char *value) {
        // LOGGER_TRACE("->%s=%s", name, value);
        ret.concat('\"');
        ret.concat(name);
        ret.concat("\":\"");
        ret.concat(value);
        ret.concat("\",");
      });
      ret += "},";
    }  // if

    l = l->next;
  }  // while


  // close root object
  ret += "}";

  // remove last comma before close.
  ret.replace(",}", "}");

  // prettify somehow
  // ret.replace("},", "},\n");

  out = ret;
}  // getState

// ===== Time functionality =====

unsigned long Board::getSeconds() {
  return (millis() / 1000);
}


// return the seconds since 1.1.1970 00:00:00
time_t Board::getTime() {
  time_t current_stamp = time(nullptr);
  if (current_stamp <= MIN_VALID_TIME) {
    current_stamp = 0;
  }  // if
  return (current_stamp);
}  // getTime()


// return the seconds of today in localtime.
time_t Board::getTimeOfDay() {
  time_t ct = time(nullptr);
  tm lt;
  if (ct) {
    localtime_r(&ct, &lt);
    return ((lt.tm_hour * 60 * 60) + (lt.tm_min * 60) + lt.tm_sec);
  } else {
    return (0);
  }
}  // getTimeOfDay()


/**
 * @brief Get a Element by typename. Returns the first found element.
 */
Element *Board::getElement(const char *elementType) {
  // LOGGER_TRACE("getElement(%s)", elementType);

  String tn = elementType;
  tn.concat(ELEM_ID_SEPARATOR);
  int tnLength = tn.length();

  Element *l = _elementList;
  while (l != NULL) {
    if (String(l->id).substring(0, tnLength).equalsIgnoreCase(tn)) {
      break;  // while
    }         // if
    l = l->next;
  }  // while
  // LOGGER_TRACE("found: %d", l);
  return (l);
}  // getElement()


Element *Board::getElement(const char *elementType, const char *elementName) {
  String tn = elementType;
  tn.concat('/');
  tn.concat(elementName);

  Element *l = _elementList;
  while (l != NULL) {
    if (String(l->id).equalsIgnoreCase(tn)) {
      break;
    }  // if
    l = l->next;
  }  // while
  return (l);

}  // getElement()


/**
 * @brief Iterate though all Elements.
 */
void Board::forEach(const char *prefix, ElementCallbackFn fCallback) {
  Element *l = _elementList;
  while (l != NULL) {
    if (Element::_stristartswith(l->id, prefix)) {
      (fCallback)(l);
    }
    l = l->next;
  }  // while
}  // forEach()


void Board::reboot(bool wipe) {
  LOGGER_INFO("reboot...");
  if (wipe)
    WiFi.disconnect(true);
  delay(1000);
#if defined(ESP8266)
  ESP.reset();
#elif defined(ESP32)
  ESP.restart();
#endif
};


void Board::displayInfo(const char *text1, const char *text2) {
  LOGGER_RAW("%s %s", text1, text2 ? text2 : "");
  if (display) {
    display->clear();
    display->drawText(0, 0, 0, text1);
    if (text2) {
      display->drawText(0, display->getLineHeight(), 0, text2);
    }
    display->flush();
  }  // if
}


// End
