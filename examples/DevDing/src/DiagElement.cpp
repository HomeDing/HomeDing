/**
 * @file DiagElement.cpp
 * @brief Element Template class.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD 3-Clause style license,
 * https://www.mathertel.de/License.aspx.
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see DiagElement.h
 */

// open http://corec3/diag
// open http://corec3/profile
// open http://corec3/chipinfo

#include <Arduino.h>
#include <HomeDing.h>
#include <hdfs.h>

#include "DiagElement.h"

#if defined(ESP32)
#include <rom/rtc.h>
#include <esp_chip_info.h>

#include <esp_heap_caps.h>
#endif

// enable TRACE for sending detailed output from this Element
#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DiagElement
 * @return DiagElement* created element
 */
Element *DiagElement::create() {
  DiagElement *e = new DiagElement();
  e->startupMode = Element::STARTUPMODE::System;
  e->loglevel = LOGGER_LEVEL_TRACE;
  return (e);
}  // create()


/* ===== Element functions ===== */

/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DiagElement::set(const char *name, const char *value) {
  bool ret = true;

  if (_stricmp(name, "rtcmem") == 0) {
    // log some heap information. using http://nodeding/api/state/diag/0?rtcmem=1
#if defined(ESP8266)
    // dump rtc Memory
    TRACE("===== RTCMEM =====");
    uint8_t rtcbuffer[16];
    for (unsigned int adr = 0; adr < (512); adr += sizeof(rtcbuffer)) {
      ESP.rtcUserMemoryRead(adr / 4, (uint32_t *)rtcbuffer, sizeof(rtcbuffer));
      String bytes;
      String chars;
      for (unsigned int n = 0; n < sizeof(rtcbuffer); n++) {
        char ch = rtcbuffer[n];
        char buf[8];
        sprintf(buf, "%02X ", ch);
        bytes.concat(buf);

        if ((ch >= 0x20) && (ch <= 0x7F)) {
          chars.concat(ch);
        } else {
          chars.concat('.');
        }
      }
      TRACE("  %04x: %s%s", adr, bytes.c_str(), chars.c_str());
    }  // for
#endif

  } else {
    ret = Element::set(name, value);
  }  // if

  return (ret);
}  // set()

// ===== web responses =====

// minimized from upload.htm
static const char diag_header[] PROGMEM =
  R"==(<!doctype html><html lang="en"><head><title>DIAG</title></head><body>)==";

// minimized from upload.htm
static const char diag_footer[] PROGMEM =
  R"==(</body></html>)==";

String DiagElement::_handleDiag() {
  String out;

  out += FPSTR(diag_header);
  out += "<h1>Diagnostics</h1>";

  out += "<pre>";
  out += "DeviceName: ";
  out += _board->deviceName;
  out += '\n';
  out += "Build Date & Time: " __DATE__ "T" __TIME__ "\n";
  out += "State: [" + DeviceState::getStateString() + "]\n";

#if defined(ESP32)
#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0))
  // ESP32 Version 3++
  out += "Mac-address: " +  ::Network.macAddress() + "\n";
#else
  // ESP32 Version 2.x
  out += "Mac-address: " + WiFi.macAddress() + "\n";
#endif
#elif defined(ESP8266)
  out += "Mac-address: " + WiFi.macAddress() + "\n";
#endif

  out += "</pre>\n";

  out += "<p><a href='/profile'>Profiler</a></p>\n";
  out += "<p><a href='/chipinfo'>Chipinfo</a></p>\n";
  out += "<p><a href='/wirescan'>I2C bus scan</a></p>\n";
  out += "<p><a href='/networks'>Networks</a></p>\n";

  out += FPSTR(diag_footer);
  return (out);
}  // _handleDiag


String DiagElement::_handleProfile() {
  String sOut;
  sOut += "Element Profile       | A | Category | Average | Maximum |   Heap |  Count\n";

  _board->forEach(CATEGORY::All, [this, &sOut](Element *e) {
    char buffer[128];
    sprintf(buffer, "%-21s | %d |   0x%04x |", e->id, e->active, e->category);
    sOut.concat(buffer);

#if defined(HD_PROFILE)
    sprintf(buffer, " %7ld | %7ld | %6ld | %6ld", (e->profile.totalDuration / e->profile.totalCount), e->profile.maxDuration, e->profile.totalMem, e->profile.totalCount);
    sOut.concat(buffer);
#endif
    sOut.concat("\n");
  });
  return (sOut);
}

String DiagElement::_handleChipInfo() {
  String sOut;
  char buffer[128];
  String out;

  sOut += FPSTR(diag_header);
  sOut += "<h1>Chip Infos</h1>";

  sOut += "<h2>SoC</h2><pre>";

#if defined(ESP8266)
  // about ESP8266 chip variants...
  sprintf(buffer, "  chip-id: 0x%08X", ESP.getChipId());
  sOut += buffer;
  sprintf(buffer, "  Flash-ID: 0x%08x\n", ESP.getFlashChipId());
  sOut += buffer;

 
#elif defined(ESP32)
  // about ESP32 chip variants...
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);

  sprintf(buffer, "model: %s(%d)\n", CONFIG_IDF_TARGET, chip_info.model);
  sOut += buffer;
  sprintf(buffer, "cores: %d\n", chip_info.cores);
  sOut += buffer;
  sprintf(buffer, "revision: %d\n", chip_info.revision);
  sOut += buffer;

  uint32_t features = chip_info.features;
  sprintf(buffer, "features: 0x%08lx\n", features);
  sOut += buffer;

  if (features & CHIP_FEATURE_EMB_FLASH) { sOut += "  embedded flash memory\n"; };
  if (features & CHIP_FEATURE_WIFI_BGN) { sOut += "  2.4GHz WiFi\n"; };
  if (features & CHIP_FEATURE_BLE) { sOut += "  Bluetooth LE\n"; };
  if (features & CHIP_FEATURE_BT) { sOut += "  Bluetooth Classic\n"; };
  if (features & CHIP_FEATURE_IEEE802154) { sOut += "  IEEE 802.15.4\n"; };

#if defined(CHIP_FEATURE_EMB_PSRAM)
  if (features & CHIP_FEATURE_EMB_PSRAM) { sOut += "  embedded psram\n"; };
#endif

#endif

  sOut += "</pre>\n";


#if defined(ESP32)
  sOut += "<h2>Flash</h2><pre>";

  sprintf(buffer, "size: %ld kByte\n", ESP.getFlashChipSize() / 1024);
  sOut += buffer;

  if (HomeDingFS::rootFS == (fs::FS *)&FFat) {
    sOut += "using FAT\n";
    sprintf(buffer, "totalBytes: %zu\n", FFat.totalBytes());
    sOut += buffer;
    sprintf(buffer, "usedBytes: %zu\n", FFat.usedBytes());
    sOut += buffer;

  } else if (HomeDingFS::rootFS == (fs::FS *)&LittleFS) {
    sOut += "using LittleFS\n";
    sprintf(buffer, "totalBytes: %zu\n", LittleFS.totalBytes());
    sOut += buffer;
    sprintf(buffer, "usedBytes: %zu\n", LittleFS.usedBytes());
    sOut += buffer;

  } else {
    sOut += "using unknown\n";
  }


#if (0)
  FlashMode_t flashMode = ESP.getFlashChipMode();
  s = "unknown";
  if (flashMode == FM_QIO) { s = "QIO"; };
  if (flashMode == FM_QOUT) { s = "QOUT"; };
  if (flashMode == FM_DIO) { s = "DIO"; };
  if (flashMode == FM_DOUT) { s = "DOUT"; };
  // if (flashMode == FM_FAST_READ) { s = "FAST_READ"; };
  // if (flashMode == FM_SLOW_READ) { s = "SLOW_READ"; };
  sprintf(buffer, "mode: %s(%d)\n", s, flashMode);
  sOut += buffer;

  sprintf(buffer, "speed: %ld\n", ESP.getFlashChipSpeed());
  sOut += buffer;
  sOut += "\n";
#endif
  sOut += "</pre>";

  sOut += "<h2>PSRAM</h2><pre>";
  sprintf(buffer, "size: %ld kByte\n", ESP.getPsramSize() / 1024);
  sOut += buffer;
  sOut += "</pre>";

#endif

  return (sOut);
}

String DiagElement::_handleWireScan() {
  TRACE("handleWireScan()");
  String sOut;
  char buffer[128];
  unsigned long tStart = millis();
  const char *desc = nullptr;
  int num = 0;
  int adr = 0;

  if ((_board->I2cSda < 0) || (_board->I2cScl < 0)) {
    sOut += "no i2c bus.\n";

  } else {
    // TRACE("Scan...");
    sprintf(buffer, "Scan i2c (sda=%d, scl=%d)...\n", _board->I2cSda, _board->I2cScl);
    sOut += buffer;

    while ((millis() < tStart + 3000) && (adr < 127)) {
      adr++;

      // TRACE("Scan 0x%02x ", adr, strlen(sOut.c_str()));
      // The i2c scanner uses the return value of Write.endTransmission
      // to find a device that acknowledged to the address.
      Wire.beginTransmission(adr);
      int error = Wire.endTransmission();

      if (error) {
        // try again for some devices that need wakeup
        Wire.beginTransmission(adr);
        error = Wire.endTransmission();
      }

      if (error == 0) {
        desc = nullptr;

        if (adr == 0x11) {
          desc = "SI4721";
        } else if (adr == (0x15)) {
          desc = "CST816D";
        } else if (adr == (0x0e)) {
          desc = "MAG3110";
        } else if (adr == (0x14)) {
          desc = "GT911";
        } else if (adr == 0x23) {
          desc = "BH1750";
        } else if (adr == 0x27) {
          desc = "LCD,PCF8574";
        } else if (adr == 0x38) {
          desc = "AHT20,FT6336";
        } else if (adr == 0x3C) {
          desc = "SH1106,SSD1306,SSD1309";
        } else if (adr == 0x3D) {
          desc = "SH1106,SSD1306";
        } else if (adr == 0x40) {
          desc = "INA219,INA226";
        } else if (adr == (0x51)) {
          desc = "RTC,PCF8563";
        } else if (adr == (0x5c)) {
          desc = "AM2320";
        } else if (adr == (0x5d)) {
          desc = "GT911";
        } else if (adr == (0x5f)) {
          desc = "HTS221";
        } else if (adr == 0x62) {
          desc = "SCD-4x";
        } else if (adr == 0x63) {
          desc = "Radio,SI4730";
        } else if (adr == (0x68)) {
          desc = "RTC,DS1307,MPU-6050";
        } else if (adr == 0x6D) {
          desc = "FBM320";
        } else if (adr == 0x77) {
          desc = "BMP280";
        }

        sprintf(buffer, "* 0x%02x: (%s)\n", adr, desc ? desc : "");
        sOut += buffer;
        yield();
        num++;
      }
    }  // while

    sprintf(buffer, "%3d addresses scanned.\n", adr);
    sOut += buffer;
    sprintf(buffer, "%3d devices found.\n", num);
    sOut += buffer;
    sOut += "</pre>";
  }
  return (sOut);
}

String DiagElement::_handleHeap() {
  // static bool inTrace = false;
  String sOut;
  char buffer[128];

  sprintf(buffer, "Free Heap: %ld\n", ESP.getFreeHeap());
  // sprintf(buffer, "=free %ld\n", esp_get_free_heap_size());
  sOut = buffer;

  return (sOut);
}

String DiagElement::_handleNetworks() {
  String sOut;

  sOut = "Local Networks:\n";

  int n = WiFi.scanNetworks();
  if (n == 0) {
    sOut += "no networks found.\n";
  } else {
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      sOut += String(i + 1);
      sOut += ": ";
      sOut += WiFi.SSID(i);
      sOut += " (";
      sOut += WiFi.RSSI(i);
      sOut += ")";
#if defined(ESP8266)
      sOut += WiFi.encryptionType(i) == ENC_TYPE_NONE ? " " : "*";
#else
      sOut += WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? " " : "*";
#endif
      delay(10);
      sOut += "\n";
    }
  }

  return (sOut);
}  // _handleNetworks()



/**
 * @brief Activate the DiagElement.
 */
void DiagElement::start() {
  TRACE("start()");
  Element::start();

  // enable I2C scan output using http://nodeding/diag
  _board->server->on("/diag", HTTP_GET, [this]() {
    _board->server->send(200, "text/html", _handleDiag());
  });

  _board->server->on("/profile", HTTP_GET, [this]() {
    _board->server->send(200, "text/plain", _handleProfile());
  });

  _board->server->on("/chipinfo", HTTP_GET, [this]() {
    _board->server->send(200, "text/html", _handleChipInfo());
  });

  _board->server->on("/wirescan", HTTP_GET, [this]() {
    _board->server->send(200, "text/plain", _handleWireScan());
  });

  _board->server->on("/networks", HTTP_GET, [this]() {
    _board->server->send(200, "text/plain", _handleNetworks());
  });

  _board->server->on("/heap", HTTP_GET, [this]() {
    _board->server->send(200, "text/plain", _handleHeap());
  });

#if defined(ESP8266)
  TRACE("Reset Reason: %s", ESP.getResetReason().c_str());
#elif defined(ESP32)
  // https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/ResetReason/ResetReason.ino
  TRACE("Reset Reason: %d", rtc_get_reset_reason(0));
#endif
}  // start()


void DiagElement::loop() {
  // no use.
  Element::loop();
}

/* ===== Register the Element ===== */

// As long as the Element is project specific or is a element always used
// the registration is placed here without using a register #define.

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_XXX should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to DiagElement.h:

// #ifdef HOMEDING_REGISTER
// Register the DiagElement onto the ElementRegistry.
bool DiagElement::registered =
  ElementRegistry::registerElement("diag", DiagElement::create);
// #endif

// End
