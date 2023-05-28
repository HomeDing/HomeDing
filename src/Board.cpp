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
#include <HomeDing.h>

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

#include <RemoteElement.h>

#include "MicroJsonParser.h"
#include "hdfs.h"

#include <DNSServer.h>

// use BOARDTRACE for compiling with detailed TRACE output.
#define BOARDTRACE(...)  // LOGGER_JUSTINFO(__VA_ARGS__)

// use NETTRACE for compiling with detailed output on startup & joining the network.
#define NETTRACE(...)  // LOGGER_JUSTINFO(__VA_ARGS__)

// time_t less than this value is assumed as not initialized.
#define MIN_VALID_TIME (30 * 24 * 60 * 60)

// The captive Mode will stay for 5 min. and then restart.
#define CAPTIVE_TIME (5 * 60 * 1000)

#define NetMode_AUTO 3  // first try
#define NetMode_PSK 2   // second try
#define NetMode_PASS 1  // last try

DNSServer *dnsServer;
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

// add you wifi network name and PassPhrase in the sketch or use WiFi Manager

extern const char *ssid;
extern const char *passPhrase;

// week definitions when not defined aqnywhere else.
const __attribute__((weak)) char *ssid = "";
const __attribute__((weak)) char *passPhrase = "";

// ---

static const char respond404[] PROGMEM =
  "<html><head><title>File not found</title></head><body>File not found</body></html>";

// define static variables for HomeDingFS here:

FS *HomeDingFS::rootFS = nullptr;
FS *HomeDingFS::sdFS = nullptr;


#if defined(ESP8266)

/** Reset Counter to detect multiple hardware resets in a row. */
int _resetCount;
bool _isWakeupStart = false;

#elif defined(ESP32)

// RTC Memory variables for counting boots in-a-row
// and detect restart after deep sleep.
RTC_DATA_ATTR int _resetCount = 0;
RTC_DATA_ATTR bool _isWakeupStart = false;


// https://community.platformio.org/t/esp32-firebeetle-fast-boot-after-deep-sleep/13206
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/kconfig.html#config-bootloader-skip-validate-in-deep-sleep
// CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP

#endif

/**
 * @brief Initialize a blank board.
 */
void Board::init(WebServer *serv, FILESYSTEM *fs, const char *buildName) {
  WiFi.persistent(true);

  LOGGER_INFO("Device %s starting...", buildName);

  server = serv;
  HomeDingFS::rootFS = fs;
  build = buildName;

  bool mounted = fs->begin();
  if (!mounted) {
    LOGGER_INFO("formatting...");
    fs->format();
  }

  Logger::init(fs);

  // board parameters configured / overwritten by device element
  sysLED = -1;
  sysButton = -1;
  homepage = "/index.htm";
  cacheHeader = "no-cache";

  _newState(BOARDSTATE::NONE);
  _deepSleepStart = 0;         // no deep sleep to be started
  _deepSleepBlock = false;     // no deep sleep is blocked
  _deepSleepTime = 60 * 1000;  // one minute
  _DeepSleepCount = 0;

#if defined(ESP8266)
  uint8_t mac[6];
  char ssid[64];

  WiFi.macAddress(mac);
  snprintf(ssid, sizeof(ssid), "ESP-%02X%02X%02X", mac[3], mac[4], mac[5]);
  deviceName = ssid;

#elif defined(ESP32)
  uint8_t mac[6];
  char ssid[64];

  WiFi.macAddress(mac);
  snprintf(ssid, sizeof(ssid), "esp32-%02x%02x%02x", mac[3], mac[4], mac[5]);
  deviceName = ssid;

#endif


#if defined(ESP8266)
  rst_info *ri = ESP.getResetInfoPtr();
  _startup = (ri->reason == REASON_DEEP_SLEEP_AWAKE) ? BOARDSTARTUP::DEEPSLEEP : BOARDSTARTUP::NORMAL;

#elif defined(ESP32)
  // RTC_DATA_ATTR int bootCount = 0;

  // https://community.platformio.org/t/esp32-firebeetle-fast-boot-after-deep-sleep/13206
  // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/kconfig.html#config-bootloader-skip-validate-in-deep-sleep
  // CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP

  esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
  LOGGER_INFO("ESP32: deep sleep cause: %d", cause);


  _startup = (_isWakeupStart) ? BOARDSTARTUP::DEEPSLEEP : BOARDSTARTUP::NORMAL;
  _isWakeupStart = false;
#endif
  if (_startup == BOARDSTARTUP::DEEPSLEEP) {
    LOGGER_INFO("Reset from Deep Sleep mode");
  }

  hd_yield();
}  // init()


void Board::add(const char *id, Element *e) {
  BOARDTRACE("add(%s)", id);
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
 * @brief Continue the Captive Mode when activity is detected.
 */
void Board::keepCaptiveMode() {
  if ((boardState == BOARDSTATE::STARTCAPTIVE) || (boardState == BOARDSTATE::RUNCAPTIVE)) {
    _captiveEnd = nowMillis + CAPTIVE_TIME;
  };
}  // keepCaptiveMode()


/**
 * @brief
 */
void Board::_checkNetState() {
  wl_status_t newState = WiFi.status();
  if (newState != _wifi_status) {
    NETTRACE("netstate: %d", newState);
    _wifi_status = newState;
  }
}


// Remark: some settings on the board class can be modified using the device element.
// see deviceElement.cpp


/**
 * @brief Add and config the Elements defined in the config files.
 */
void Board::_addAllElements() {
  BOARDTRACE("addAllElements()");
  Element *_lastElem = NULL;  // last created Element

  MicroJson *mj = new MicroJson(
    [this, &_lastElem](int level, char *path, char *value) {
      // BOARDTRACE("callback %d %s =%s", level, path, value ? value : "-");
      hd_yield();

      if (level == 1) {

      } else if (level == 2) {
        // create new element
        BOARDTRACE("new %s", path);
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
        BOARDTRACE(" (%s) %s=%s", path, name, value ? value : "-");
        // add a parameter to the last Element
        _lastElem->set(name, value);
      }  // if
    });

  if (mj) {
    // config the thing to the local network
    mj->parseFile(HomeDingFS::rootFS, ENV_FILENAME);
    hd_yield();

    // config the Elements of the device
    mj->parseFile(HomeDingFS::rootFS, CONF_FILENAME);
    hd_yield();
  }  // if

  delete mj;
}  // _addAllElements()


void Board::start(Element_StartupMode startupMode) {
  // BOARDTRACE("start(%d)", startupMode);

  // make elements active that match
  Element *l = _elementList;
  while (l) {
    if ((!l->active) && (l->startupMode <= startupMode)) {
      // start element when not already active
      BOARDTRACE("starting %s...", l->id);
      l->setup();  // one-time initialization
      l->start();  // start...
      hd_yield();
    }  // if

    l = l->next;
  }  // while

  active = true;
  _nextElement = nullptr;
  _activeElement = nullptr;
}  // start()


// switch to a new state
void Board::_newState(enum BOARDSTATE newState) {
  hd_yield();
  NETTRACE("WiFi: %d,%d", WiFi.getMode(), WiFi.status());
  NETTRACE("New State=%d", newState);
  boardState = newState;
}

// loop next element, only one at a time!
void Board::loop() {
  static String netpass;

  nowMillis = millis();

  if (boardState != BOARDSTATE::RUN) {
    _checkNetState();
  }

  if (boardState == BOARDSTATE::RUN) {
    // Most common state first.

#if defined(ESP8266)
    if (_startup == BOARDSTARTUP::NORMAL) {
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

    // dispatch next action from _actions if any
    if (!_actions.empty()) {
      _DeepSleepCount = 0;
      String a = _actions.pop();
      BOARDTRACE("popped %s", a.c_str());
      dispatchAction(a);
      _actions.dump();
      return;
    } else {
    }  // if

    // give some time to next active element
    if (_nextElement == NULL) {
      _nextElement = _elementList;
    }  // if
    if (_nextElement) {
      if (_nextElement->active) {
        // BOARDTRACE("loop %s", _nextElement->id);
#if defined(HD_TRACE)
        TRACE_START(_nextElement);
#endif
        _activeElement = _nextElement;
        _nextElement->loop();
        _activeElement = nullptr;
#if defined(HD_TRACE)
        TRACE_END(_nextElement);
#endif
      }
      _nextElement = _nextElement->next;
    }  // if

    if ((!_deepSleepBlock) && (_deepSleepStart > 0)) {
      _DeepSleepCount++;

      // deep sleep specified time.
      if ((nowMillis > _deepSleepStart) && (_DeepSleepCount > _addedElements + 4)) {
        // all elements now had the chance to create and dispatch an event.
        LOGGER_INFO("sleep %ld msecs.", _deepSleepTime);
        Serial.flush();
#if defined(ESP32)
        // remember there was deep sleep mode before reset.
        _isWakeupStart = true;
#endif
        ESP.deepSleep(_deepSleepTime * 1000);
        _newState(BOARDSTATE::SLEEP);
      }
    }  // if

  } else if (boardState == BOARDSTATE::NONE) {
    // load network connection details
    File f = HomeDingFS::rootFS->open(NET_FILENAME, "r");
    if (f) {
      netpass = f.readString();
      f.close();
    }
    _newState(BOARDSTATE::LOAD);

  } else if (boardState == BOARDSTATE::LOAD) {
    // load all config files and create+start elements
    _addAllElements();
    BOARDTRACE("Elements created.");
    if (!_elementList) {
      Logger::logger_level = LOGGER_LEVEL_TRACE;
      // no element defined, so allow configuration in any case.
      isSafeMode = false;

      // start up simple device and ota
      add("device/0", ElementRegistry::createElement("device"));
      add("ota/0", ElementRegistry::createElement("ota"));
    }
    _checkNetState();

    if (state) {
      // when a state element is configured: use it to load state
      state->load();
    }

    if (sysButton >= 0) {
      pinMode(sysButton, INPUT_PULLUP);
    }

#if defined(ESP8266)
    _resetCount = RTCVariables::getResetCounter();
#endif
    if (_resetCount > 0) {
      // enforce un-safemode on double reset
      LOGGER_INFO("Reset #%d", _resetCount);
      isSafeMode = false;
    }  // if
    _resetCount++;
#if defined(ESP8266)
    RTCVariables::setResetCounter(_resetCount);
#endif

    // search any time requesting elements
    Element *l = _elementList;
    while (l != nullptr) {
      if (l->startupMode == Element_StartupMode::Time) {
        hasTimeElements = true;
      }  // if
      l = l->next;
    }  // while

    // setup I2C system wide when configured.
    if ((I2cSda >= 0) && (I2cScl >= 0)) {
      LOGGER_TRACE("I2C pins sda=%d scl=%d", I2cSda, I2cScl);
      Wire.begin(I2cSda, I2cScl);
      if (I2cFrequency > 0) {
        Wire.setClock(I2cFrequency);
      }
    }

    start(Element_StartupMode::System);
    displayInfo(HOMEDING_GREETING);

    // Enable sysLED for blinking while waiting for network or config mode
    if (sysLED >= 0) {
      pinMode(sysLED, OUTPUT);
      digitalWrite(sysLED, HIGH);
    }

    LOGGER_TRACE("WiFi.SSID=%s", WiFi.SSID().c_str());
    LOGGER_TRACE("$net=%s", ListUtils::at(netpass, 0).c_str());
    LOGGER_TRACE("resetCount=%d", _resetCount);

    // detect no configured network situation
    if ((WiFi.SSID().length() == 0) && (strnlen(ssid, 2) == 0) && netpass.isEmpty()) {
      LOGGER_JUSTINFO("no config");
      _newState(BOARDSTATE::STARTCAPTIVE);  // start hotspot right now.

      // } else if (_resetCount == 1) {
      //   // no safe mode

    } else if (_resetCount == 2) {
      LOGGER_JUSTINFO("Reset*2");
      _newState(BOARDSTATE::STARTCAPTIVE);  // start hotspot right now.

    } else if (WiFi.SSID().length() || netpass.length()) {
      NETTRACE("Auto");
      netMode = NetMode_AUTO;
      _newState(BOARDSTATE::CONNECT);

    } else {
      NETTRACE("Pass");
      netMode = NetMode_PASS;
      _newState(BOARDSTATE::CONNECT);
    }

    // wait at least some seconds for offering config mode
    configPhaseEnd = nowMillis + minConfigTime;


  } else if (boardState == BOARDSTATE::CONNECT) {
    BOARDTRACE("connect...");

    WiFi.mode(WIFI_STA);
    if (deviceName.isEmpty()) {
      BOARDTRACE("no deviceName configured");
    } else {
      WiFi.setHostname(deviceName.c_str());
    }
    // get effective Hostname
    deviceName = WiFi.getHostname();
    BOARDTRACE("deviceName=%s", deviceName.c_str());

#if defined(ESP8266)
    WiFi.setOutputPower(outputPower);
#endif
    WiFi.setAutoReconnect(true);
    _newState(BOARDSTATE::WAITNET);

    if (netMode == NetMode_AUTO) {
      // 1. try:
      // give autoconnect the chance to do it.
      // works only after a successfull network connection in the past.
      WiFi.setAutoConnect(true);
      WiFi.begin();
      NETTRACE("NetMode_AUTO: %s", WiFi.SSID().c_str());

    } else if (netMode == NetMode_PSK) {
      // 2. try:
      // connect with the saved network and password
      int off = netpass.indexOf(',');
      WiFi.begin(netpass.substring(0, off).c_str(), netpass.substring(off + 1).c_str());
      NETTRACE("NetMode_PSK <%s>,<%s>", netpass.substring(0, off).c_str(), netpass.substring(off + 1).c_str());

    } else if (netMode == NetMode_PASS) {
      // 3. try:
      // use fixed network and passPhrase known at compile time.
      NETTRACE("NetMode_PASS: %s", ssid);

      if (!*ssid) {
        NETTRACE("SKIP");
        connectPhaseEnd = 0;
        return;
      } else {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, passPhrase);
      }
    }  // if

    // wait some(12) seconds for connecting to the network
    connectPhaseEnd = nowMillis + maxNetConnextTime;

  } else if ((boardState == BOARDSTATE::WAITNET) || (boardState == BOARDSTATE::WAIT)) {
    // make sysLED blink.
    // short pulses for normal=safemode, long pulses for unsafemode.
    if (sysLED >= 0) {
      digitalWrite(sysLED, (nowMillis % 700) > (isSafeMode ? 100 : 600) ? HIGH : LOW);
    }  // if

    // check sysButton
    if ((sysButton >= 0) && (digitalRead(sysButton) == LOW)) {
      LOGGER_JUSTINFO("sysbutton %d pressed %d", sysButton, digitalRead(sysButton));
      _newState(BOARDSTATE::STARTCAPTIVE);
    }

    if (boardState == BOARDSTATE::WAITNET) {
      if (_wifi_status == WL_CONNECTED) {
        NETTRACE("connected.");
        _newState(BOARDSTATE::WAIT);
      }  // if

      if ((_wifi_status == WL_NO_SSID_AVAIL) || (_wifi_status == WL_CONNECT_FAILED) || (nowMillis > connectPhaseEnd)) {

        if (!connectPhaseEnd) {
          // no TRACE;
        } else if (nowMillis > connectPhaseEnd) {
          NETTRACE("timed out.");
        } else {
          NETTRACE("wifi status=(%d)", _wifi_status);
        }  // if

        netMode -= 1;
        // NETTRACE("next connect method = %d\n", netMode);
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
      if ((_startup == BOARDSTARTUP::DEEPSLEEP) || (nowMillis >= configPhaseEnd)) {
        _newState(BOARDSTATE::GREET);
      }
    }  // if
    hd_yield();


  } else if (boardState == BOARDSTATE::GREET) {
    // after network connection is done.
    _resetCount = RTCVariables::setResetCounter(0);

    const char *name = WiFi.getHostname();

    displayInfo(name, WiFi.localIP().toString().c_str());
    LOGGER_JUSTINFO("connected to %s (%s mode)",
                    WiFi.SSID().c_str(), (isSafeMode ? "safe" : "unsafe"));
    LOGGER_JUSTINFO("start http://%s/", name);

    // Serial.println();
    // WiFi.printDiag(Serial);
    // NETTRACE("WiFi.RSSI: %d", WiFi.RSSI());
    // Serial.println();

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
    }

    server->begin();
    server->enableCORS(true);

    randomSeed(millis());         // millis varies on every start, good enough
    filesVersion = random(8000);  // will incremented on every file upload by file server

    if (cacheHeader == "etag") {
#if defined(ESP8266)
      // enable eTags in results for static files
      // by setting "cache": "etag" inc env.json on the device element

      // This is a fast custom eTag generator. It returns a current number that gets incremented when any file is updated.
      server->enableETag(true, [this](FS &fs, const String &path) -> String {
        String eTag;
        if (!path.endsWith(".txt")) {
          // txt files contain logs that must not be cached.
          // eTag = esp8266webserver::calcETag(fs, path);
          File f = fs.open(path, "r");
          eTag = String(f.getLastWrite(), 16);  // use file modification timestamp to create ETag
          f.close();
          // use current counter
          // eTag = String(filesVersion, 16);  // f.getLastWrite()
        }
        return (eTag);
      });
#endif
      cacheHeader = "";  // do not pass this cache header
    }

    start(Element_StartupMode::Network);
    dispatch(sysStartAction);  // dispatched when network is available

    // ===== finish network setup

    // start mDNS service discovery for "_homeding._tcp"
    // but not when using deep sleep mode
    if (_startup == BOARDSTARTUP::DEEPSLEEP) {
      _mDnsEnabled = false;
    }
    if (_mDnsEnabled) {
      BOARDTRACE("startup mdns... %s", deviceName.c_str());

#if defined(ESP8266)
      MDNS.begin(deviceName.c_str());
      // include the data required for the portal implementation: Overview of existing devices
      MDNSResponder::hMDNSService serv;

      serv = MDNS.addService(0, "http", "tcp", 80);
      MDNS.addServiceTxt(serv, "path", homepage.c_str());

      serv = MDNS.addService(0, "homeding", "tcp", 80);
      MDNS.addServiceTxt(serv, "path", homepage.c_str());
      MDNS.addServiceTxt(serv, "title", title.c_str());
      MDNS.addServiceTxt(serv, "room", room.c_str());

#elif defined(ESP32)
      MDNS.begin(deviceName.c_str());
      MDNS.addService("homeding", "tcp", 80);
      MDNS.addServiceTxt("homeding", "tcp", "path", homepage.c_str());
      MDNS.addServiceTxt("homeding", "tcp", "title", title.c_str());
      MDNS.addServiceTxt("homeding", "tcp", "room", room.c_str());
#endif
    }  // if


    // ===== initialize network dependant services

    // start file server for static files in the file system.
    server->serveStatic("/", *(HomeDingFS::rootFS), "/", cacheHeader.c_str());

    server->onNotFound([this]() {
      BOARDTRACE("notFound: %s", server->uri().c_str());
      server->send(404, "text/html", FPSTR(respond404));
    });

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

    // setup dns server using standard port
    dnsServer = new DNSServer();
    dnsServer->setTTL(300);
    dnsServer->setErrorReplyCode(DNSReplyCode::ServerFailure);
    dnsServer->start(0, "*", apIP);

    server->begin();

    _newState(BOARDSTATE::RUNCAPTIVE);
    keepCaptiveMode();

  } else if (boardState == BOARDSTATE::RUNCAPTIVE) {
    // server.handleClient(); needs to be called in main loop.
    dnsServer->processNextRequest();

    // make sysLED blink 3 sec with a short flash.
    if (sysLED >= 0) {
      digitalWrite(sysLED, ((nowMillis % 3000) > 120) ? HIGH : LOW);
    }  // if

    if (nowMillis > _captiveEnd) {
      LOGGER_INFO("no config");
      reboot(false);
    }
  }  // if
}  // loop()


// ===== set board behavior

// start deep sleep mode for given duration when idle.
void Board::setSleepTime(unsigned long milliSeconds) {
  BOARDTRACE("setSleepTime(%d)", milliSeconds);
  _deepSleepTime = milliSeconds;
}  // setSleepTime()


// start deep sleep mode when idle.
void Board::startSleep() {
  BOARDTRACE("startSleep");
  _deepSleepStart = millis();
  if (_startup == BOARDSTARTUP::NORMAL) {
    // give a minute time to block deep sleep mode
    _deepSleepStart += 60 * 1000;
  }
}  // startSleep()


// block any deep sleep until next reset.
void Board::cancelSleep() {
  BOARDTRACE("cancelSleep");
  _deepSleepBlock = true;
}  // cancelSleep()


Element *Board::findById(String &id) {
  return (findById(id.c_str()));
}

Element *Board::findById(const char *id) {
  // BOARDTRACE("findById(%s)", id);

  Element *l = _elementList;
  while (l != NULL) {
    if (strcmp(l->id, id) == 0) {
      // BOARDTRACE(" found:%s", l->id);
      break;  // while
    }         // if
    l = l->next;
  }  // while
  return (l);
}  // findById


// ===== queue / process / dispatch actions =====

bool Board::queueIsEmpty() {
  return (_actions.empty());
}


/** Queue an action for later dispatching. */
void Board::_queueAction(const String &action, const String &v, boolean split) {
  if (!action.isEmpty()) {
    String tmp = action;
    tmp.replace("$v", v);
    LOGGER_INFO("(%s)=>(%s)", (_nextElement ? _nextElement->id : ""), tmp.c_str());
    if (split) {
      int len = ListUtils::length(tmp);
      for (int n = 0; n < len; n++) {
        _actions.push(ListUtils::at(tmp, n).c_str());
      }
    } else {
      _actions.push(tmp);
    }
  }
}  // _queueAction


// send a event out to the defined target.
void Board::dispatchAction(String action) {
  BOARDTRACE("dispatch %s", action.c_str());
  // TRACE_START;

  // [host:](type/id)?(param)[=val]
  String host, targetId, name, value;

  int p = action.indexOf(':');
  int pParam = action.indexOf('?');

  if ((p > 0) && (p < pParam)) {
    host = action.substring(0, p);
    action.remove(0, p + 1);
    pParam -= (p + 1);
  }

  if (pParam > 0) {
    targetId = action.substring(0, pParam);
    targetId.toLowerCase();
  } else {
    LOGGER_ERR("no action");
    return;
  }

  int pValue = action.indexOf(ELEM_VALUE, pParam + 1);
  if (pValue > 0) {
    name = action.substring(pParam + 1, pValue);
    value = action.substring(pValue + 1);
  } else {
    name = action.substring(pParam + 1);
    value = "";
  }

  if (!host.isEmpty()) {
    // host:type/id?param=val
    // send action over the network to host
    String remoteID = "remote/" + host;
    RemoteElement *target = (RemoteElement *)findById(remoteID);

    if (!target) {
      LOGGER_ERR("dispatch: %s not found", remoteID.c_str());
    } else {
      // also show action in log when target has trace loglevel
      target->dispatchAction(targetId, name, value);
    }

  } else {
    Element *target = findById(targetId);

    if (!target) {
      LOGGER_ERR("dispatch: %s not found", targetId.c_str());

    } else {
      // also show action in log when target has trace loglevel
      Logger::LoggerEPrint(target, LOGGER_LEVEL_TRACE, "action(%s)", action.c_str());
      bool ret = target->set(name.c_str(), value.c_str());

      if (!ret)
        LOGGER_ERR("Action '%s' was not accepted.", action.c_str());
    }
  }
  // TRACE_END;
  // TRACE_TIMEPRINT("used time:", "", 25);
}  // dispatchAction()


/**
 * @brief Save an action to the _actionList.
 */
void Board::dispatch(const String &action, int value) {
  _queueAction(action, String(value));
}  // dispatch


/**
 * @brief Save an action to the _actionList.
 */
void Board::dispatch(const String &action, const char *value) {
  _queueAction(action, String(value));
}  // dispatch


/**
 * @brief Save an action to the _actionList.
 */
void Board::dispatch(const String &action, const String &value, boolean split) {
  _queueAction(action, value, split);
}  // dispatch


/**
 * @brief Save an action to the _actionList using a item part of a value.
 */
void Board::dispatchItem(const String &action, const String &values, int n) {
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
  _DeepSleepCount = 0;
}  // deferSleepMode()


void Board::getState(String &out, const String &path) {
  // BOARDTRACE("getState(%s)", path.c_str());
  String ret = "{";
  const char *cPath = path.c_str();

  Element *l = _elementList;
  while (l != NULL) {
    // BOARDTRACE("  ->%s", l->id);
    if ((cPath[0] == '\0') || (strcmp(l->id, cPath) == 0)) {
      ret += '\"';
      ret += l->id;
      ret += "\":{";
      l->pushState([&ret](const char *name, const char *value) {
        // BOARDTRACE("->%s=%s", name, value);
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
  hd_yield();
  time_t ct = time(nullptr);

  if (ct > 0) {
    struct tm *lt;
    lt = localtime(&ct);
    return ((lt->tm_hour * 60 * 60) + (lt->tm_min * 60) + lt->tm_sec);
  } else {
    return (0);
  }
}  // getTimeOfDay()


/**
 * @brief Get a Element by typename. Returns the first found element.
 */
Element *Board::getElement(const char *elementType) {
  // BOARDTRACE("getElement(%s)", elementType);

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
  // BOARDTRACE("found: %d", l);
  return (l);
}  // getElement()


/**
 * @brief Get a Element by typename/id.
 */
Element *Board::getElementById(const char *elementId) {
  BOARDTRACE("getElementById(%s)", elementId);

  Element *l = _elementList;
  while (l != NULL) {
    if (Element::_stricmp(l->id, elementId) == 0) { break; }
    l = l->next;
  }  // while
  // BOARDTRACE("found: 0x%08x", l);
  return (l);
}  // getElementById()


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


/**
 * @brief Reset/restart the board.
 * @param wipe is set to true to disconnect from WiFi and forget saved network credentials.
 */
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
  LOGGER_ALWAYS("%s %s", text1, text2 ? text2 : "");
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
