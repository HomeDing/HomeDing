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

#include <Wire.h>
#include <SPI.h>

#include <core/Network.h>

#if defined(ESP8266)
#include <ESP8266mDNS.h>
extern "C" {
#include <user_interface.h>  // https://github.com/esp8266/Arduino actually tools/sdk/include
}
#include "sntp.h"

#elif defined(ESP32)
#include <ESPmDNS.h>
#include <rom/rtc.h>
#endif

#include <ElementRegistry.h>

#include <RemoteElement.h>

#include "MicroJsonParser.h"
#include "hdfs.h"

#include <DNSServer.h>

using namespace HomeDing::Actions;

// use BOARDTRACE for compiling with detailed TRACE output.
#define BOARDTRACE(...)  // Logger::LoggerPrint("Board", LOGGER_LEVEL_TRACE, __VA_ARGS__)

// use ELEM-TRACE for compiling with detailed TRACE output for Element creation.
#define ELEMTRACE(...)  // Logger::LoggerPrint("Elem", LOGGER_LEVEL_TRACE, __VA_ARGS__)

// use NETTRACE for compiling with detailed output on startup & joining the network.
#define NETTRACE(...)  // Logger::LoggerPrint("Net", LOGGER_LEVEL_TRACE, __VA_ARGS__)

// time_t less than this value is assumed as not initialized.
#define MIN_VALID_TIME (30 * 24 * 60 * 60)

// The captive Mode will stay for 10 min. and then restart.
#define CAPTIVE_TIME (10 * 60 * 1000)

DNSServer *dnsServer;
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

unsigned long saveWorkingState = 0;

// add you wifi network name and PassPhrase in the sketch or use WiFi Manager

extern const char *ssid;
extern const char *passPhrase;

// week definitions when not defined anywhere else.
const __attribute__((weak)) char *ssid = "";
const __attribute__((weak)) char *passPhrase = "";

int _resetCount = 0;

// ---

static const char respond404[] PROGMEM =
  "<html><head><title>File not found</title></head><body>File not found</body></html>";

// define static variables for HomeDingFS here:

FS *HomeDingFS::rootFS = nullptr;
FS *HomeDingFS::sdFS = nullptr;


/** Reset Counter to detect multiple hardware resets in a row. */
bool _isWakeupStart = false;

/**
 * @brief Initialize a blank board.
 */
void Board::init(WebServer *serv, FILESYSTEM *fs, const char *buildName) {
  bool mounted = false;

  server = serv;
  HomeDingFS::rootFS = fs;
  build = buildName;

  Logger::init(fs);
  Network::init();

  Logger::printf("Device %s starting...", buildName);
#if defined(ESP32)
  BOARDTRACE("Reset Reasons: %d %d", rtc_get_reset_reason(0), rtc_get_reset_reason(1));
#endif

#if defined(ESP8266)
  mounted = LittleFS.begin();
  if (!mounted) {
    Logger::printf("formatting...");
    mounted = fs->format();
  }

#elif defined(ESP32)
  if (fs == (fs::FS *)&FFat) {
    mounted = FFat.begin(true);

  } else if (fs == (fs::FS *)&LittleFS) {
    mounted = LittleFS.begin(true);
  }
#endif

  if (!mounted) {
    Logger::printf("FS failed.");
  }

  DeviceState::load();

  // board parameters configured / overwritten by device element
  homepage = "/index.htm";
  cacheHeader = "etag";  //  "no-cache";

  _newBoardState(BOARDSTATE::NONE);
  _deepSleepStart = 0;         // no deep sleep to be started
  _deepSleepBlock = false;     // no deep sleep is blocked
  _deepSleepTime = 60 * 1000;  // one minute
  _DeepSleepCount = 0;

  // create a devicename from mac address
  uint8_t mac[6];
  char sMac[64];

#if defined(ESP32)
#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0))
  // ESP32 Version 3++
  Network.macAddress(mac);
  snprintf(sMac, sizeof(sMac), "esp32-%02x%02x%02x", mac[3], mac[4], mac[5]);

#else
  // ESP32 Version 2.x
  WiFi.macAddress(mac);
  snprintf(sMac, sizeof(sMac), "esp32-%02x%02x%02x", mac[3], mac[4], mac[5]);
#endif

#elif defined(ESP8266)
  WiFi.macAddress(mac);
  snprintf(sMac, sizeof(sMac), "esp-%02x%02x%02x", mac[3], mac[4], mac[5]);
#endif

  deviceName = sMac;

  // check for deep sleep wakeup

#if defined(ESP8266)
  rst_info *ri = ESP.getResetInfoPtr();
  _startup = (ri->reason == REASON_DEEP_SLEEP_AWAKE) ? BOARDSTARTUP::DEEPSLEEP : BOARDSTARTUP::NORMAL;

#elif defined(ESP32)
  // RTC_DATA_ATTR int bootCount = 0;

  // https://community.platformio.org/t/esp32-firebeetle-fast-boot-after-deep-sleep/13206
  // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/kconfig.html#config-bootloader-skip-validate-in-deep-sleep
  // CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP

  BOARDTRACE("ws=%d", _isWakeupStart);
  BOARDTRACE("ESP32: deep sleep cause: %d", esp_sleep_get_wakeup_cause());

  _startup = (_isWakeupStart) ? BOARDSTARTUP::DEEPSLEEP : BOARDSTARTUP::NORMAL;
  _isWakeupStart = false;
#endif

  if (_startup == BOARDSTARTUP::DEEPSLEEP) {
    LOGGER_INFO("Reset from Deep Sleep mode");
  }

  hd_yield();
}  // init()


void Board::add(const char *id, Element *e) {
  ELEMTRACE("add(%s)", id);
  bool hasLoop = (e->category & Element::CATEGORY::Looping);
  Element *l = hasLoop ? _elementList : _elementListNoLoop;

  _addedElements++;
  strcpy(e->id, id);
  Element::_strlower(e->id);

  // append to end of list.
  if ((!l) && hasLoop) {
    _elementList = e;  // first looping element

  } else if ((!l) && (!hasLoop)) {
    _elementListNoLoop = e;  // first non-looping element

  } else {
    // search last Element.
    while (l->next)
      l = l->next;
    // append.
    l->next = e;
  }  // if
  e->next = nullptr;
  e->init(this);
}  // add()


/// @brief Check the mode the device in running.
/// @return true when board is runing in captive mode.
/// @return false when board is runing in normal mode.
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
  Network::loop();
}


// Remark: some settings on the board class can be modified using the device element.
// see deviceElement.cpp


/**
 * @brief Add and config the Elements defined in the config files.
 */
void Board::_addAllElements() {
  ELEMTRACE("addAllElements()");
  Element *_lastElem = NULL;  // last created Element

  MicroJson *mj = new MicroJson(
    [this, &_lastElem](int level, char *_path, char *value) {
      ELEMTRACE("callback %d %s =%s", level, _path, value ? value : "-");
      hd_yield();

      char path[128];
      strncpy(path, _path, sizeof(path));
      strlwr(path);

      if (level == 1) {

      } else if (level == 2) {
        // create new element

        char typeName[32];

        char *p = path;
        char *t = typeName;
        while (*p && *p != MICROJSON_PATH_SEPARATOR) {
          *t++ = *p++;
        }  // while
        *t = '\0';

        // typeName starts with "web" ?
        if (Element::_strStartsWith(typeName, "web")) {
          // don't try to create web elements
          _lastElem = nullptr;
        } else {
          _lastElem = ElementRegistry::createElement(typeName);
          if (!_lastElem) {
            LOGGER_ERR("No such Element: %s", typeName);
          } else {
            // add to the list of elements
            add(path, _lastElem);
          }
        }  // if

      } else if ((level > 2) && (_lastElem) && (value)) {
        // Search 2. slash as starting point for name to include 3. level
        char *name = strchr(path, MICROJSON_PATH_SEPARATOR) + 1;
        name = strchr(name, MICROJSON_PATH_SEPARATOR) + 1;
        dispatchAction(_lastElem, name, value ? value : "-");
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


void Board::start(Element::STARTUPMODE startupMode) {
  // BOARDTRACE("start(%d)", startupMode);

  // make elements active that match the startupMode
  forEach(Element::CATEGORY::All, [startupMode](Element *e) {
    if ((!e->active) && (e->startupMode <= startupMode)) {
      // start element when not already active
      ELEMTRACE("starting %s...", e->id);
      e->setup();  // one-time initialization
      e->start();  // start...
      hd_yield();
    }
  });

  active = true;
  _nextElement = nullptr;
  _activeElement = nullptr;
}  // start()


// switch to a new state
void Board::_newBoardState(enum BOARDSTATE newState) {
  hd_yield();
  // NETTRACE("WiFi state: %d", WiFi.status());
  boardState = newState;
  BOARDTRACE("New State=%d", newState);
}

// loop next element, only one at a time!
void Board::loop() {
  Network::loop();

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
          start(Element::STARTUPMODE::Time);
          startComplete = true;
        }  // if
      }  // if

      if (startComplete) {
        HomeDing::Actions::push(startAction);  // dispatched when all elements are active.
        saveWorkingState = nowMillis + (10 * 1000);
      }

    } else if (saveWorkingState) {
      if (saveWorkingState < nowMillis) {
        BOARDTRACE("Device is up and running...");
        // some time after startComplete has completed.
        DeviceState::setResetCounter(0);
        saveWorkingState = 0;
      }
    }  // if


    // dispatch next action from queue if any
    if (!HomeDing::Actions::queueIsEmpty()) {
      _DeepSleepCount = 0;
      dispatchAction(HomeDing::Actions::pop());
      return;
    }  // if

    // no actions left
    // trigger flush to a display
    if ((display) && display->startFlush(false)) {
      return;
    }  // if

    // give some time to next active element from the Looping List
    if (!_nextElement) {
      _nextElement = _elementList;
    }  // if

    if (_nextElement) {
      if ((_nextElement->active) && (_nextElement->category & Element::CATEGORY::Looping)) {
        // BOARDTRACE("loop %s", _nextElement->id);
#if defined(HD_PROFILE)
        PROFILE_START(_nextElement);
#endif
        _activeElement = _nextElement;
        _nextElement->loop();
        _activeElement = nullptr;
#if defined(HD_PROFILE)
        PROFILE_END(_nextElement);
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
        Logger::flush();
#if defined(ESP32)
        // remember there was deep sleep mode before reset.
        _isWakeupStart = true;
#endif
        ESP.deepSleep(_deepSleepTime * 1000);
        _newBoardState(BOARDSTATE::SLEEP);
      }
    }  // if

  } else if (boardState == BOARDSTATE::NONE) {
    // load network connection details
    _newBoardState(BOARDSTATE::LOAD);

  } else if (boardState == BOARDSTATE::LOAD) {

    netpass = "";
    if ((ssid) && (*ssid)) {
      // use hard coded ssid+passPhrase
      LOGGER_TRACE("$net: use hard coded network.");
      netpass = ssid;
      netpass.concat(',');
      if (passPhrase) netpass.concat(passPhrase);

    } else {
      // load network connection details
      File f = HomeDingFS::rootFS->open(NET_FILENAME, "r");
      if (f) {
        netpass = f.readString();
        f.close();
      }
    }

    netpass.replace("\n", "");
    NETTRACE("$net=<%s>", ListUtils::at(netpass, 0).c_str());

    // increment BootCount
    _resetCount = DeviceState::getResetCounter() + 1;
    DeviceState::setResetCounter(_resetCount);

    if (_resetCount > 1)
      Logger::printf("reset #%d", _resetCount);

    if ((!netpass.isEmpty()) && (_resetCount <= 3))
      _newBoardState(BOARDSTATE::SETUP);
    else
      _newBoardState(BOARDSTATE::STARTCAPTIVE);


  } else if (boardState == BOARDSTATE::SETUP) {

    // load all config files and create elements
    if (_resetCount < 3) {
      BOARDTRACE("Create Elements...");
      _addAllElements();
    }

    if (_resetCount == 2) {
      // enforce un-safemode on double reset
      isSafeMode = false;
    }

    if (!_elementList) {
      BOARDTRACE("Create SYS-Elements...");
      Logger::logger_level = LOGGER_LEVEL_TRACE;
      // no element defined, so allow configuration in any case.
      isSafeMode = false;

      // start up simple device and ota
      add("device/0", ElementRegistry::createElement("device"));
      add("ota/0", ElementRegistry::createElement("ota"));
    }
    BOARDTRACE("Create Elements done.");

    _checkNetState();

    // startup normal modes...
    if (_resetCount < 3) {
      // load state data and actions
      DeviceState::loadElementState(this);

      // search any time requesting elements
      forEach(Element::CATEGORY::All, [this](Element *e) {
        if (e->startupMode == Element::STARTUPMODE::Time) {
          hasTimeElements = true;
        }
      });

      // setup I2C system wide when configured.
      if ((I2cSda >= 0) && (I2cScl >= 0)) {
        LOGGER_TRACE("I2C pins sda=%d scl=%d", I2cSda, I2cScl);
        Wire.begin(I2cSda, I2cScl);
        if (I2cFrequency > 0) {
          Wire.setClock(I2cFrequency);
        }
      }

#if defined(ESP32)
      // setup SPI system wide when configured.
      if (spiCLK >= 0) {
        LOGGER_TRACE("SPI clk=%d miso=%d mosi=%d", spiCLK, spiMISO, spiMOSI);
        SPI.begin(spiCLK, spiMISO, spiMOSI);
      }
#endif
    }

    start(Element::STARTUPMODE::System);  // including displays !
    displayInfo(HOMEDING_GREETING);

    _newBoardState(BOARDSTATE::CONNECT);

  } else if (boardState == BOARDSTATE::CONNECT) {
    int off = netpass.indexOf(',');
    String nName = netpass.substring(0, off);
    String nPass = netpass.substring(off + 1);

    // connect to a network...
    Network::connect(deviceName, nName, nPass);

    // get effective Hostname
    deviceName = WiFi.getHostname();

#if defined(ESP8266)
    WiFi.setOutputPower(outputPower);
#endif

    _checkNetState();

    _newBoardState(BOARDSTATE::WAITNET);

    // wait some(12) seconds for connecting to the network
    connectPhaseEnd = nowMillis + maxNetConnectTime;

  } else if (boardState == BOARDSTATE::WAITNET) {
    bool needReset = false;

    if (Network::state == Network::NETSTATE::CONNECTED) {
      NETTRACE("connected.");
      _newBoardState(BOARDSTATE::GREET);

    } else if (Network::state == Network::NETSTATE::FAILED) {
      NETTRACE("connecting failed.");
      needReset = true;

    } else if (nowMillis > connectPhaseEnd) {
      NETTRACE("connecting timed out.");
      needReset = true;

    } else if (Network::state == Network::NETSTATE::CONNECTSTA) {
      // wait on...
      delay(100);

    }  // if

    if (needReset) {
      displayInfo("no-net restart");
      DeviceState::setResetCounter(0);

      delay(250);
      ESP.restart();
    }  // if
    hd_yield();

  } else if (boardState == BOARDSTATE::GREET) {
    const char *name = WiFi.getHostname();

    displayInfo(name, WiFi.localIP().toString().c_str());
    Logger::printf("connected to %s (%s mode)",
                   WiFi.SSID().c_str(), (isSafeMode ? "safe" : "unsafe"));
    Logger::printf("start http://%s/\n", name);

    if (display) {
      delay(1600);
      display->clear();
      display->startFlush(true);
    }  // if

    server->begin();
    server->enableCORS(true);

    randomSeed(millis());  // millis varies on every start, good enough

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
        }
        return (eTag);
      });

#elif defined(ESP32)

#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0))
      // enable eTags in results for static files
      // by setting "cache": "etag" inc env.json on the device element
      // don't cache *.txt files

      // This is a fast custom eTag generator. It returns a current number that gets incremented when any file is updated.
      server->enableETag(true, [this](FS &fs, const String &path) -> String {
        String eTag;
        if (!path.endsWith(".txt")) {
          File f = fs.open(path, "r");
          eTag = String(f.getLastWrite(), 16);  // use file modification timestamp to create ETag
          f.close();
        }
        return (eTag);
      });
#endif

#endif
      cacheHeader = "";  // do not pass this cache header
    }

    start(Element::STARTUPMODE::Network);
    HomeDing::Actions::push(sysStartAction);  // dispatched when network is available

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
      MDNS.announce();

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

    _newBoardState(BOARDSTATE::RUN);
  } else if (boardState == BOARDSTATE::SLEEP) {
    // just wait.

  } else if (boardState == BOARDSTATE::STARTCAPTIVE) {
    uint8_t mac[6];
    char ssid[64];

    DeviceState::setResetCounter(0);

#if defined(ESP32)
#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0))
    // ESP32 Version 3++
    Network.macAddress(mac);
#else
    // ESP32 Version 2.x
    WiFi.macAddress(mac);
#endif

#elif defined(ESP8266)
    WiFi.macAddress(mac);
#endif





    snprintf(ssid, sizeof(ssid), "%s%02X%02X%02X", HOMEDING_GREETING, mac[3], mac[4], mac[5]);

    displayInfo("Captive Mode:", ssid);

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP(ssid);
    delay(100);
    BOARDTRACE(" AP-IP: %s", WiFi.softAPIP().toString().c_str());

    // setup dns server using standard port
    dnsServer = new DNSServer();
    dnsServer->setTTL(300);
    dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer->start(53, "*", apIP);

    server->begin();

    _newBoardState(BOARDSTATE::RUNCAPTIVE);
    keepCaptiveMode();

  } else if (boardState == BOARDSTATE::RUNCAPTIVE) {
    // server.handleClient(); needs to be called in main loop.
    dnsServer->processNextRequest();

    if (nowMillis > _captiveEnd) {
      Logger::printf("no config.");
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
  LOGGER_INFO("start sleep");
  _deepSleepStart = millis();
  if (_startup == BOARDSTARTUP::NORMAL) {
    // give a minute time to block deep sleep mode
    _deepSleepStart += 60 * 1000;
  }
}  // startSleep()


// block any deep sleep until next reset.
void Board::cancelSleep() {
  LOGGER_INFO("cancel sleep");
  _deepSleepBlock = true;
}  // cancelSleep()


// ===== dispatch actions =====

// send a event out to the defined target.
void Board::dispatchAction(Element *target, const char *action_name, const char *action_value) {

  if (target) {
    const char *action = HomeDing::Actions::find(action_name);
    if (!action) action = action_name;

#if defined(LOGGER_ENABLED)
    // show action in log when target has trace loglevel
    // Logger::LoggerEPrint(target, LOGGER_LEVEL_TRACE, "#set %s=%s", action_name, action_value);
    if (Logger::logger_level >= LOGGER_LEVEL_TRACE)
      Logger::printf("#set    %s?%s=%s", target->id, action, action_value);

#if defined(HD_PROFILE)
    PROFILE_START(target);
#endif
    bool ret = target->set(action, action_value);
#if defined(HD_PROFILE)
    PROFILE_END(target);
#endif

    if (!ret) {
      LOGGER_ERR("Action '%s' was not accepted by %s.", action, target->id);
    }

#else
    target->set(action, action_value);
#endif
  }
}  // dispatchAction()


// send a event out to the defined target.
void Board::dispatchAction(String action) {
  BOARDTRACE("dispatch %s", action.c_str());

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
  name.toLowerCase();

  if (!host.isEmpty()) {
    // host:type/id?param=val
    // send action over the network to host
    String remoteID = "remote/" + host;
    RemoteElement *target = (RemoteElement *)findById(remoteID);

    if (!target) {
      LOGGER_ERR("dispatch: %s not found", remoteID.c_str());
    } else {
      // send over the network to target device
      target->dispatchAction(targetId, name, value);
    }

  } else {
    Element *target = findById(targetId);

    if (!target) {
      LOGGER_ERR("dispatch: %s not found", targetId.c_str());

    } else {
      dispatchAction(target, name.c_str(), value.c_str());
    }
  }
}  // dispatchAction()



// ===== low power / sleep mode =====

/**
 * do not start sleep mode because element is active.
 */
void Board::deferSleepMode() {
  // reset the counter to ensure looping all active elements
  _DeepSleepCount = 0;
}  // deferSleepMode()


void Board::getState(String &out, const char *id) {
  BOARDTRACE("getState(%s)", id ? id : "-");
  String ret = "{";

  forEach(Element::CATEGORY::All, [this, id, &ret](Element *e) {
    BOARDTRACE("  %s", e->id);
    if ((!id) || (strcmp(e->id, id) == 0)) {
      ret += '\"';
      ret += e->id;
      ret += "\":{";
      e->pushState([&ret](const char *name, const char *value) {
        BOARDTRACE("->%s=%s", name, value);
        ret.concat('\"');
        ret.concat(name);
        ret.concat("\":\"");
        ret.concat(value);
        ret.concat("\",");
      });
      ret += "},";
    }  // if
  });

  // close root object and remove last comma before close.
  ret += "}";
  ret.replace(",}", "}");
  out = ret;
  BOARDTRACE("getState=%s", out.c_str());
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


Element *Board::getElement(const char *elementType, const char *elementName) {
  char id[32];
  sprintf(id, "%s/%s", elementType, elementName);
  return (findById(id));
}  // getElement()


Element *Board::findById(const char *id) {
  BOARDTRACE("findById(%s)", id ? id : "-");
  Element *found = nullptr;

  if (id) {
    forEach(Element::CATEGORY::All, [id, &found](Element *e) {
      if (strcmp(e->id, id) == 0) { found = e; }
    });
  }
  return (found);
}  // findById


/// @brief Iterate all Elements from both lists with a given category.
void Board::forEach(Element::CATEGORY cat, ElementCallbackFn fCallback) {
  Element *l;

  l = _elementList;
  while (l != NULL) {
    if (l->category & cat) {
      (fCallback)(l);
    }
    l = l->next;
  }  // while

  l = _elementListNoLoop;
  while (l != NULL) {
    if (l->category & cat) {
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
  Logger::printf("reboot...");
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
  Logger::printf("%s %s", text1, text2 ? text2 : "");
  if (display) {
    display->clear();
    BoundingBox b = display->drawText(0, 0, 0, text1, HomeDing::displayConfig.drawColor);

    if (text2) {
      int16_t y = b.y_max + 1;
      if (b.y_max > 8) { y += 4; }
      display->drawText(0, y, 0, text2, HomeDing::displayConfig.drawColor);
    }
    display->startFlush(true);
  }  // if
}


// End
