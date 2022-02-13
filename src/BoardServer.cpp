/**
 * @file boardServer.cpp
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

// http://nodeding/api/sysinfo
// http://nodeding/api/elements
// http://nodeding/api/list
// http://nodeding/api/state
// http://nodeding/api/state/device/0
// http://nodeding/api/state/device/0?title=over

// http://nodeding/api/reboot
// http://nodeding/api/-reset
// http://nodeding/api/-resetall
// http://nodeding/api/-cleanweb

// http://nodeding/api/scan
// http://nodeding/api/connect?n=netname&p=passwd

// http://nodeding/ // redirect to defined start page


#include <Arduino.h>
#include <Board.h>
#include <Element.h>

#include <BoardServer.h>
#include <ElementRegistry.h>

#include <MicroJsonComposer.h>

// used for built-in files
#define SVC_ANY "/$"

// used for services
#define API_ROUTE "/api/"

// integrated htm files
#define PAGE_UPDATE_VERS "/$update.htm#v03"

// Content types for http results
#define TEXT_JSON "application/json; charset=utf-8" // Content type for JSON.
#define TEXT_PLAIN "text/plain; charset=utf-8" // Content type for txt and empty results.


// use TRACE for compiling with detailed TRACE output.
#define TRACE(...) // LOGGER_JUSTINFO(__VA_ARGS__)

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
      millis() + (60 * 1000); // TODO: make configurable
  server.send(200);

  if (server.hasArg("n")) {
    // const char *netName = server.arg("n").c_str();
    String netName = server.arg("n");
    String netPass = server.arg("p");
    TRACE("connect %s/%s", netName.c_str(), netPass.c_str());

    _board->displayInfo("setup network", netName.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.begin(netName.c_str(), netPass.c_str());

  } else if (server.hasArg("wps")) {
    // TODO:WPS start using wps
    // WiFi.beginWPSConfig();
  } // if

  while (connectTimeout > millis()) {
    delay(100);
    wl_status_t wifi_status = WiFi.status();
    // TRACE("WiFi.status=%d", wifi_status);

    if (wifi_status == WL_CONNECTED) {
      WiFi.persistent(true);
      WiFi.setAutoConnect(true);
      _board->displayInfo("ok.");
      break;
    } else if ((wifi_status == WL_NO_SSID_AVAIL) || (wifi_status == WL_CONNECT_FAILED)) {
      _board->displayInfo("failed.");
      WiFi.disconnect(true);
      break;
    } // if

  } // while
  delay(400);
  _board->reboot(false);
} // handleConnect()


// Return list of local networks.
// TODO:refactor return result as string. no server send here.
String BoardHandler::handleScan() {
  TRACE("handleScan");
  String result;

  int8_t scanState = WiFi.scanComplete();
  if (scanState == WIFI_SCAN_FAILED) {
    // restart an async network scan
    WiFi.scanNetworks(true);

  } else if (scanState != WIFI_SCAN_RUNNING) {
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
} // handleScan()


// reset or reboot the device
void BoardHandler::handleReboot(WebServer &server, bool wipe) {
  server.send(200);
  server.client().stop();
  _board->reboot(wipe);
} // handleReboot()


/**
 * @brief Verify if the method and requestUri can be handles by this module.
 * @param requestMethod current http request method.
 * @param requestUri current url of the request.
 * @return true When the method and requestUri match a state request.
 */
#if defined(ESP8266)
bool BoardHandler::canHandle(HTTPMethod requestMethod, const String &requestUri)
#elif defined(ESP32)
bool BoardHandler::canHandle(HTTPMethod requestMethod, String requestUri)
#endif
{
  bool can = ((requestMethod == HTTP_GET) // only GET requests in the API
              && (requestUri.startsWith(SVC_ANY) // old api entries
                  || requestUri.startsWith(API_ROUTE) // new api entries
                  || (requestUri == "/"))); // handle redirect
  return (can);
} // canHandle


/**
 * @brief Handle the request of the state.
 * @param server reference to the server.
 * @param requestMethod current http request method.
 * @param requestUri current url of the request.
 * @return true When the state could be retrieved.
 * @return false
 */
#if defined(ESP8266)
bool BoardHandler::handle(WebServer &server, UNUSED HTTPMethod requestMethod, const String &requestUri2)
#elif defined(ESP32)
bool BoardHandler::handle(WebServer &server, HTTPMethod requestMethod, String requestUri2)
#endif
{
  TRACE("handle(%s)", requestUri2.c_str());
  String output;
  const char *output_type = nullptr; // when output_type is set then send output as response.
  FILESYSTEM *fs = _board->fileSystem;

  bool ret = true;
  bool unSafeMode = !_board->isSafeMode;
  String uri = requestUri2;
  String api;

  uri.toLowerCase();
  output.reserve(512);

  if (uri.startsWith(SVC_ANY)) {
    api = uri.substring(2);

  } else if (uri.startsWith(API_ROUTE)) {
    api = uri.substring(5);
  }

  if ((api == "board") || (api == "state")) {
    // most common request, return state of all elements
    _board->getState(output, String());
    output_type = TEXT_JSON;

  } else if ((api.startsWith("state/")) || (api.startsWith("board/"))) {
    // everything behind  "/api/state/" is used to address a specific element
    String id(api.substring(6));

    // TRACE("state/(%s)", uri.c_str());
    int argCount = server.args();

    if (argCount == 0) {
      // get status of all or the specified element
      _board->getState(output, id);

    } else {
      // send all actions to the specified element per given argument
      for (int a = 0; a < argCount; a++) {
        String tmp = id + "?" + server.argName(a) + "=$v";
        _board->dispatch(tmp, server.arg(a));
      }
    } // if
    output_type = TEXT_JSON;

  } else if (api == "sysinfo") {
    unsigned long now = millis();
    MicroJsonComposer jc;

    jc.openObject();
    jc.addProperty("devicename", _board->deviceName);
    jc.addProperty("build", __DATE__);
    jc.addProperty("freeHeap", ESP.getFreeHeap());

    jc.addProperty("flashSize", ESP.getFlashChipSize());
    jc.addProperty("coreVersion", String(_board->version));
    jc.addProperty("coreBuild", String(_board->build));

#if defined(ESP8266)
    FSInfo fs_info;
    fs->info(fs_info);
    jc.addProperty("fsTotalBytes", fs_info.totalBytes);
    jc.addProperty("fsUsedBytes", fs_info.usedBytes);
#elif defined(ESP32)
    jc.addProperty("fsTotalBytes", fs->totalBytes());
    jc.addProperty("fsUsedBytes", fs->usedBytes());
#endif
    jc.addProperty("safemode", _board->isSafeMode ? "true" : "false");
    jc.addProperty("upTime", now / 1000);

    // WIFI info
    jc.addProperty("ssid", WiFi.SSID());

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
    fs->format();
    handleReboot(server, true);
    output_type = TEXT_PLAIN;

  } else if (unSafeMode && (api == "cleanweb")) {
    // remove files but configuration
    handleCleanWeb("/");
    output_type = TEXT_PLAIN;

  } else if (unSafeMode && (api == "list")) {
    // List files in filesystem
    MicroJsonComposer jc;
    jc.openArray();
    handleListFiles(jc, "/");
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

  } else if (uri == "/") {
    // handle a redirect to the defined homepage or system system / update
    TRACE("Redirect...");
    String url;

    if (_board->isCaptiveMode()) {
      url = "http://";
      url.concat(WiFi.softAPIP().toString()); // _board->deviceName
      url.concat("/$setup");

    } else {
      url = _board->homepage;

#if defined(ESP8266)
      FSInfo fsi;
      fs->info(fsi);
      if (fsi.usedBytes < 18000) {
        // assuming UI files not installed
        url = PAGE_UPDATE_VERS;
        if (fsi.totalBytes < 500000) {
          // small file system
          url.concat('m');
        }
      }

#elif defined(ESP32)
      if (fs->usedBytes() < 18000) {
        // assuming UI files not installed
        url = PAGE_UPDATE_VERS;
      }
#endif

    } // if
    server.sendHeader("Location", url, true);
    server.send(302);

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
} // handle()


#if defined(ESP8266) // && defined(SPIFFS)

// ESP8266 SPIFFS implementation, compatible with LittleFS on ESP8266
// list files in filesystem recursively.

void listDirectory(MicroJsonComposer &jc, FILESYSTEM *fs, String path, Dir dir) {
  while (dir.next()) {
    String name = dir.fileName();

    if (dir.isDirectory()) {
      Dir subDir = fs->openDir(path + name);
      listDirectory(jc, fs, path + name + "/", subDir);
    } else {
      jc.openObject();
      jc.addProperty("type", "file");
      String longName = path + name;
      longName.replace("//", "/");
      jc.addProperty("name", longName);
      jc.addProperty("size", dir.fileSize());
      jc.closeObject();
    } // if
  } // while
}

void BoardHandler::handleListFiles(MicroJsonComposer &jc, String path) {
  FILESYSTEM *fs = _board->fileSystem;
  Dir dir = fs->openDir(path);
  listDirectory(jc, fs, "/", dir);
}


#elif defined(ESP32)

// list files in filesystem recursively.
void BoardHandler::handleListFiles(MicroJsonComposer &jc, String path) {
  FILESYSTEM *fs = _board->fileSystem;
  File dir = fs->open(path, "r");
  TRACE("handleListFiles(%s)", path.c_str());
  // ASSERT: last char of path = '/'

  while (File entry = dir.openNextFile()) {
    String name = path + entry.name();

    if ((name.indexOf('#') >= 0) || (name.indexOf('$') >= 0)) {
      // do not report as file

    } else if (entry.isDirectory()) {
      // jc.openObject();
      // jc.addProperty("type", "dir");
      // jc.addProperty("name", name);
      // jc.closeObject();
      handleListFiles(jc, name + "/");

    } else {
      jc.openObject();
      jc.addProperty("type", "file");
      jc.addProperty("name", name);
      jc.addProperty("size", entry.size());
      // jc.addProperty("time", entry.getLastWrite());
      jc.closeObject();
    } // if
  } // while
} // handleListFiles()

#endif


void BoardHandler::handleCleanWeb(String path) {
  FILESYSTEM *fSys = _board->fileSystem;
  LOGGER_JUSTINFO("handleCleanWeb(%s)", path.c_str());
  // ASSERT: last char of path = '/'

  fs::File dir = fSys->open(path, "r");
  while (File entry = dir.openNextFile()) {
    String name = path + entry.name();
    LOGGER_JUSTINFO("is: %s", name.c_str());

    if (entry.isDirectory()) {
      LOGGER_JUSTINFO("isDir.");
      handleCleanWeb((name + "/"));
    } else {
      LOGGER_JUSTINFO("isFile.");
      // fSys->remove(name);
    } // if
  } // while
} // handleCleanWeb()


// End.