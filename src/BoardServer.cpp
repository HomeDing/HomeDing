/**
 * @file BoardServer.cpp
 * @brief Implementation of a web server request hander to handle the IoT board
 * REST services.
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
 * Changelog: see BoardServer.h
 */

// http://homeding/api/sysinfo
// http://homeding/api/elements
// http://homeding/api/list
// http://homeding/api/state
// http://homeding/api/state/device/0
// http://homeding/api/state/device/0?title=over

// http://homeding/api/reboot
// http://homeding/api/-reset
// http://homeding/api/-resetall
// http://homeding/api/-cleanweb

// http://homeding/api/scan
// http://homeding/api/connect?n=netname&p=passwd

// http://homeding/ // redirect to defined start page


#include <Arduino.h>
#include <HomeDing.h>

#include <BoardServer.h>
#include <ElementRegistry.h>

#include <MicroJsonComposer.h>
#include <hdfs.h>

// used for services
#define API_ROUTE "/api/"

// integrated htm files
#define PAGE_UPDATE "/$update.htm"

// Content types for http results
#define TEXT_JSON "application/json; charset=utf-8"  // Content type for JSON.
#define TEXT_PLAIN "text/plain; charset=utf-8"       // Content type for txt and empty results.


// use TRACE for compiling with detailed TRACE output.
#define TRACE(...)  // LOGGER_JUSTINFO(__VA_ARGS__)

/**
 * @brief Construct a new State Handler object
 * @param board reference to the board.
 */
BoardHandler::BoardHandler(Board *board) {
  TRACE("BoardHandler:init");
  _board = board;
}

// ===== board specific services

/** Use url parameters to establish / verify a WiFi connection. */
void BoardHandler::handleConnect(WebServer &server) {
  TRACE("handleConnect");
  unsigned long connectTimeout =
    millis() + (60 * 1000);  // TODO: make configurable
  String netName, netPass;
  server.send(200);

  if (server.hasArg("f")) {
    LOGGER_JUSTINFO("format...");
    HomeDingFS::format();
  }

  if (server.hasArg("n")) {
    // const char *netName = server.arg("n").c_str();
    netName = server.arg("n");
    netPass = server.arg("p");
    TRACE("connect %s/%s", netName.c_str(), netPass.c_str());

    _board->displayInfo("setup network", netName.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.begin(netName.c_str(), netPass.c_str());

    // } else if (server.hasArg("wps")) {
    // TODO:WPS start using wps
    // WiFi.beginWPSConfig();
  }  // if

  while (connectTimeout > millis()) {
    delay(100);
    wl_status_t wifi_status = WiFi.status();
    // TRACE("WiFi.status=%d", wifi_status);

    if (wifi_status == WL_CONNECTED) {
      WiFi.persistent(true);
      _board->displayInfo("ok.");

      File f = HomeDingFS::open(NET_FILENAME, "w");
      if (f) {
        f.printf("%s,%s", netName.c_str(), netPass.c_str());
        f.close();
      }
      break;

    } else if ((wifi_status == WL_NO_SSID_AVAIL) || (wifi_status == WL_CONNECT_FAILED)) {
      _board->displayInfo("failed.");
      WiFi.disconnect(true);
      break;
    }  // if

  }  // while
  delay(400);
  _board->reboot(false);
}  // handleConnect()


// Return list of local networks.
// TODO:refactor return result as string. no server send here.
String BoardHandler::handleScan() {
  TRACE("handleScan");
  String result;

  _board->keepCaptiveMode();

  int8_t scanState = WiFi.scanComplete();
  TRACE("handleScan state=%d", scanState);

  if (scanState == WIFI_SCAN_FAILED) {
    // restart an async network scan
    WiFi.scanNetworks(true);

  } else if (scanState == WIFI_SCAN_RUNNING) {
    // nothing but waiting.

  } else if (scanState > 0) {
    // return scan result
    MicroJsonComposer jc;
    jc.openArray();

    for (int i = 0; i < scanState; i++) {
      jc.openObject();
      jc.addProperty("id", WiFi.SSID(i));
      jc.addProperty("rssi", WiFi.RSSI(i));
#if defined(ESP8266)
      jc.addProperty("open", WiFi.encryptionType(i) == ENC_TYPE_NONE);
#elif defined(ESP32)
      jc.addProperty("open", WiFi.encryptionType(i) == WIFI_AUTH_OPEN);
#endif
      jc.closeObject();
    }
    jc.closeArray();
    WiFi.scanDelete();
    result = jc.stringify();
  }
  return (result);
}  // handleScan()


// reset or reboot the device
void BoardHandler::handleReboot(WebServer &server, bool wipe) {
  TRACE("handleReboot(%d)", wipe);
  server.send(200);
  server.client().stop();
  _board->reboot(wipe);
}  // handleReboot()


/**
 * @brief Verify if the method and uri can be handles by this module.
 * @param requestMethod current http request method.
 * @param uri current url of the request.
 * @return true When the method and uri match a state request.
 */
#if defined(ESP8266)
bool BoardHandler::canHandle(HTTPMethod requestMethod, const String &uri)
#elif defined(ESP32)

#if (ESP_ARDUINO_VERSION_MAJOR < 3)
bool BoardHandler::canHandle(HTTPMethod requestMethod, String uri)
#else
bool BoardHandler::canHandle(WebServer &server, HTTPMethod requestMethod, const String &uri)
#endif

#endif
{
  TRACE("BoardHandler::canhandle(%s)", uri.c_str());
  // LOGGER_JUSTINFO("HTTP: > %s", uri.c_str());
  bool can = ((requestMethod == HTTP_GET)          // only GET requests in the API
              && (uri.startsWith(API_ROUTE)        // new api entries
                  || (uri == "/")                  // handle redirect
                  || (_board->isCaptiveMode())));  // capt

  TRACE("  %d", can);
  return (can);
}  // canHandle


/**
 * @brief Handle the request of the state.
 * @param server reference to the server.
 * @param requestMethod current http request method.
 * @param requestUri current url of the request.
 * @return true When the state could be retrieved.
 * @return false
 */
#if defined(ESP8266)
bool BoardHandler::handle(WebServer &server, HTTPMethod /* requestMethod */, const String &requestUri2)
#elif defined(ESP32)
bool BoardHandler::handle(WebServer &server, HTTPMethod /* requestMethod */, const String &requestUri2)
#endif
{
  TRACE("BoardHandler::handle(%s)", requestUri2.c_str());
  String output;
  const char *output_type = nullptr;  // when output_type is set then send output as response.

  bool ret = true;
  bool unSafeMode = !_board->isSafeMode;
  String uri = requestUri2;
  String api;

  uri.toLowerCase();

  if (uri.startsWith(API_ROUTE)) {
    api = uri.substring(5);
  }

  if (api == "state") {
    // most common request, return state of all elements
    _board->getState(output);
    output_type = TEXT_JSON;

  } else if (api.startsWith("state/")) {
    // everything behind  "/api/state/" is used to address a specific element
    String id(api.substring(6));

    // TRACE("state/(%s)", uri.c_str());
    int argCount = server.args();

    if (argCount == 0) {
      // get status of the specified element
      _board->getState(output, id.c_str());

    } else {
      // send arguments as actions to the specified element per given argument
      for (int a = 0; a < argCount; a++) {
        String tmp = id + "?" + server.argName(a) + "=$v";
        HomeDing::Actions::push(tmp, server.arg(a), false);
      }
    }  // if
    output_type = TEXT_JSON;

  } else if (api == "sysinfo") {
    unsigned long now = millis();
    MicroJsonComposer jc;

    jc.openObject();
    jc.addProperty("devicename", _board->deviceName);
    jc.addProperty("coreVersion", String(_board->version));
    jc.addProperty("coreBuild", String(_board->build));

#if defined(ESP8266)
    jc.addProperty("core", "ESP8266");

#elif defined(ESP32)
    jc.addProperty("core", ESP.getChipModel());
#endif

    jc.addProperty("build", __DATE__);
    jc.addProperty("freeHeap", ESP.getFreeHeap());

#if !defined(HD_MINIMAL)
    jc.addProperty("flashSize", ESP.getFlashChipSize());
    jc.addProperty("mac", WiFi.macAddress().c_str());
#endif

#if defined(ESP8266)
    // ESP32 supports LittleFS only
    FSInfo fs_info;
    LittleFS.info(fs_info);
    jc.addProperty("fsTotalBytes", fs_info.totalBytes);
    jc.addProperty("fsUsedBytes", fs_info.usedBytes);

#elif defined(ESP32)
    // ESP32 supports FFat and LittleFS
    if (HomeDingFS::rootFS == (fs::FS *)&FFat) {
      jc.addProperty("fsTotalBytes", FFat.totalBytes());
      jc.addProperty("fsUsedBytes", FFat.usedBytes());
    } else {
      jc.addProperty("fsTotalBytes", LittleFS.totalBytes());
      jc.addProperty("fsUsedBytes", LittleFS.usedBytes());
    }

#endif
    jc.addProperty("safemode", _board->isSafeMode ? "true" : "false");
    jc.addProperty("upTime", now / 1000);

#if !defined(HD_MINIMAL)
    // WIFI info
    jc.addProperty("ssid", WiFi.SSID());
#endif

    jc.closeObject();
    output = jc.stringify();
    output_type = TEXT_JSON;

    // ===== restarting modes
  } else if (api == "reboot") {
    // no reset of parameters, just reboot
    handleReboot(server, false);

  } else if (unSafeMode && (api == "reset")) {
    // Reset network parameters and reboot
    handleReboot(server, true);

  } else if (unSafeMode && (api == "resetall")) {
    // Reset file system, network parameters and reboot
    TRACE("  formatting...");
    HomeDingFS::format();
    handleReboot(server, true);
    output_type = TEXT_PLAIN;

  } else if (unSafeMode && (api == "cleanweb")) {
    // remove files but configuration
    handleCleanWeb("/");
    output_type = TEXT_PLAIN;

  } else if (unSafeMode && (api == "list")) {
    // List files in filesystem
    String p = server.arg("path");
    TRACE("path=%s", p.c_str());

    if (!p.endsWith("/")) { p += '/'; }

    MicroJsonComposer jc;
    jc.openArray();
    handleListFiles(jc, p);
    jc.closeArray();
    output = jc.stringify();
    output_type = TEXT_JSON;

    // ===== Connection Manager services
  } else if (unSafeMode && (api == "scan")) {
    output = handleScan();
    output_type = output.length() ? TEXT_JSON : TEXT_PLAIN;

  } else if (unSafeMode && (api == "connect")) {
    handleConnect(server);
    // no output_type. connection is already lost here

  } else if (unSafeMode && (api == "elements")) {
    output = ElementRegistry::list();
    output_type = TEXT_JSON;

  } else if ((uri == "/") && (!_board->isCaptiveMode())) {
    // handle a redirect to the defined homepage or system system / update
    LOGGER_JUSTINFO("Redirect...");
    String url = _board->homepage;
    size_t used = 0;

#if defined(ESP8266)
    FSInfo fsi;
    LittleFS.info(fsi);
    used = fsi.usedBytes;

#elif defined(ESP32)
    // ESP32 supports FFat and LittleFS
    if (HomeDingFS::rootFS == (fs::FS *)&FFat) {
      used = FFat.usedBytes();
    } else {
      used = LittleFS.usedBytes();
    }
#endif
    if (used < 18000) {
      url = PAGE_UPDATE;  // assuming UI files not installed
    }
    server.sendHeader("Location", url, true);
    server.send(302);


  } else if (_board->isCaptiveMode()) {
    LOGGER_JUSTINFO(">> $setup...");

    // Handle Redirect in Captive Portal Mode
    // redirect requests like "/generate_204", "/gen_204", "/chat", /connecttest.txt
    // to WIFI setup dialog /$setup

    String url = "/$setup";
    server.sendHeader("Location", url, true);
    server.send(302);  // Found

  } else {
    TRACE("uri:%s", uri.c_str());
    ret = false;
  }

  if (output_type) {
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("X-Content-Type-Options", "no-sniff");
    server.send(200, output_type, output);
  }

  return (ret);
}  // handle()


// list files in filesystem, one folder only.
void BoardHandler::handleListFiles(MicroJsonComposer &jc, String path) {
  TRACE("handleListFiles(%s)", path.c_str());

  if ((path.length() > 1) && (path.endsWith("/")))
    path.remove(path.length() - 1, 1);  // last '/'

  File dir = HomeDingFS::open(path);

  if (HomeDingFS::sdFS && path.equals("/")) {
    // add the /sd mount folder to the list
    jc.openObject();
    jc.addProperty("type", "dir");
    jc.addProperty("name", SD_MOUNTNAME);
    jc.closeObject();
  }

  if ((path.length() > 1) && (! path.endsWith("/")))
    path.concat('/');  // last '/'

  while (File entry = dir.openNextFile()) {
    String longName = path + entry.name();
    TRACE("  + %s", longName.c_str());

    if ((longName.indexOf('#') >= 0) || (longName.indexOf('$') >= 0)) {
      // do not report as a file

    } else {
      jc.openObject();
      jc.addProperty("name", longName);

      if (entry.isDirectory()) {
        jc.addProperty("type", "dir");
      } else {
        jc.addProperty("size", entry.size());
      }
      jc.closeObject();
    }  // if
  }  // while
}  // handleListFiles()


void BoardHandler::handleCleanWeb(String path) {
  TRACE("handleCleanWeb(%s)", path.c_str());

  File dir = HomeDingFS::open(path, "r");

  while (File entry = dir.openNextFile()) {
    String longName = path + entry.name();
    TRACE("is: %s", longName.c_str());

    if (entry.isDirectory()) {
      TRACE("isDir.");
      handleCleanWeb((longName + "/"));

    } else if ((longName.indexOf('#') >= 0) || (longName.indexOf('$') >= 0)) {
      TRACE("isSecret.");

    } else if (longName.indexOf(".json") >= 0) {
      TRACE("isConfig.");

    } else {
      TRACE("isFile.");
      HomeDingFS::remove(longName);
    }  // if
  }  // while
}  // handleCleanWeb()


// End.