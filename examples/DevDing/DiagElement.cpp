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

// open http://nodeding/diag

#include <Arduino.h>
#include <HomeDing.h>

#include "DiagElement.h"

#if defined(ESP32)
#include <rom/rtc.h>
#endif

#if !defined(TRACE)
#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)
#endif

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DiagElement
 * @return DiagElement* created element
 */
Element *DiagElement::create() {
  return (new DiagElement());
}  // create()


/* ===== Element functions ===== */

DiagElement::DiagElement() {
  startupMode = Element_StartupMode::System;
}


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


String DiagElement::_handleDiag() {
  String out;
  char buffer[128];
  const char *desc = nullptr;
  unsigned long tStart = millis();

  out += "**Info**\n";
  out += "DeviceName: ";
  out += _board->deviceName;
  out += '\n';
  out += "Build Date & Time: " __DATE__ "T" __TIME__ "\n";
  out += "\n";

  sprintf(buffer, "Scan i2c (sda=%d, scl=%d)...\n", _board->I2cSda, _board->I2cScl);
  out += buffer;

  int num = 0;
  int adr = 1;

  while ((millis() < tStart + 3000) && (adr < 127)) {

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
      } else if (adr == (0x0e)) {
        desc = "MAG3110";
      } else if (adr == (0x14)) {
        desc = "GT911";
      } else if (adr == 0x23) {
        desc = "BH1750";
      } else if (adr == 0x27) {
        desc = "LCD,PCF8574";
      } else if (adr == 0x38) {
        desc = "AHT20";
      } else if (adr == 0x3C) {
        desc = "SH1106,SSD1306,SSD1309";
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
      out += buffer;
      yield();
      num++;
    }
    adr++;
  }  // while

  sprintf(buffer, "%3d adresses scanned.\n", adr);
  out += buffer;
  sprintf(buffer, "%3d devices found.\n", num);
  out += buffer;
  return (out);
}  // _handleDiag



void DiagElement::_logChipDetails() {
  TRACE("Chip-Info:");
  const char *s;

#if defined(ESP8266)
  // about ESP8266 chip variants...
  TRACE("  chip-id: 0x%08X", ESP.getChipId());
#elif defined(ESP32)
  // about ESP32 chip variants...
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);

  esp_chip_model_t model = chip_info.model;
  s = "unknown";
  if (model == CHIP_ESP32) { s = "ESP32"; };
  if (model == CHIP_ESP32S2) { s = "ESP32-S2"; };
  if (model == CHIP_ESP32S3) { s = "ESP32-S3"; };
  if (model == CHIP_ESP32C3) { s = "ESP32-C3"; };
  if (model == CHIP_ESP32H2) { s = "ESP32-H2"; };
  TRACE("  model: %s(%d)", s, model);

  uint32_t features = chip_info.features;
  TRACE("  features: %08x", features);
  if (features & CHIP_FEATURE_EMB_FLASH) { TRACE("    embedded flash memory"); };
  if (features & CHIP_FEATURE_WIFI_BGN) { TRACE("    2.4GHz WiFi"); };
  if (features & CHIP_FEATURE_BLE) { TRACE("    Bluetooth LE"); };
  if (features & CHIP_FEATURE_BT) { TRACE("    Bluetooth Classic"); };
  if (features & CHIP_FEATURE_IEEE802154) { TRACE("    IEEE 802.15.4"); };
#if defined(CHIP_FEATURE_EMB_PSRAM)
  if (features & CHIP_FEATURE_EMB_PSRAM) { TRACE("    embedded psram"); };
#endif

  TRACE("  cores: %d", chip_info.cores);
  TRACE("  revision: %d", chip_info.revision);

  TRACE("ChipModel: %s", ESP.getChipModel());
#endif

  TRACE("Flash:");
#if defined(ESP8266)
  TRACE("  ID: 0x%08x", ESP.getFlashChipId());
#endif
  TRACE("  Size: %d kByte", ESP.getFlashChipSize() / 1024);

  FlashMode_t flashMode = ESP.getFlashChipMode();
  s = "unknown";
  if (flashMode == FM_QIO) { s = "QIO"; };
  if (flashMode == FM_QOUT) { s = "QOUT"; };
  if (flashMode == FM_DIO) { s = "DIO"; };
  if (flashMode == FM_DOUT) { s = "DOUT"; };
  // if (flashMode == FM_FAST_READ) { s = "FAST_READ"; };
  // if (flashMode == FM_SLOW_READ) { s = "SLOW_READ"; };
  TRACE("  Mode: %s(%d)", s, flashMode);
  TRACE("  Speed: %d", ESP.getFlashChipSpeed());
}

/**
 * @brief Activate the DiagElement.
 */
void DiagElement::start() {
  TRACE("start()");
  Element::start();

  // enable I2C scan output using http://nodeding/diag
  _board->server->on("/diag", HTTP_GET, [this]() {
    _board->server->send(200, "text/plain", _handleDiag());
  });

  TRACE("I2C pins sda=%d scl=%d", _board->I2cSda, _board->I2cScl);

#if defined(ESP8266)
  TRACE("Reset Reason: %s", ESP.getResetReason().c_str());
#elif defined(ESP32)
  // https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/ResetReason/ResetReason.ino
  TRACE("Reset Reason: %d", rtc_get_reset_reason(0));
#endif

  TRACE(" Free Heap: %d", ESP.getFreeHeap());
  TRACE(" Mac-address: %s", WiFi.macAddress().c_str());

  _logChipDetails();
}  // start()


/**
 * @brief push the current value of all properties to the callback.
 */
void DiagElement::pushState(
  std::function<void(const char *pName, const char *eValue)> callback) {
  Element::pushState(callback);
}  // pushState()


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
