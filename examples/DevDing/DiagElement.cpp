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

#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)

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
    // log some heap information. using http://nodeding/$board/diag/0?rtcmem=1
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


String DiagElement::_scanI2C() {
  String out;
  char buffer[128];
  const char *desc = nullptr;
  unsigned long tStart = millis();

  sprintf(buffer, "Scan i2c (sda=%d, scl=%d)...\n", _board->I2cSda, _board->I2cScl);
  out += buffer;

  int num = 0;
  int adr = 1;

  while ((millis() < tStart + 3000) && (adr < 127)) {

    // The i2c scanner uses the return value of Write.endTransmission
    // to find a device that acknowledged to the address.
    LOGGER_ETRACE("s=%02x", adr);
    Wire.beginTransmission(adr);
    int error = Wire.endTransmission();

    if (error) {
      // try again for some devices that need wakeup
      Wire.beginTransmission(adr);
      error = Wire.endTransmission();
    }

    if (error == 0) {
      sprintf(buffer, "* 0x%02x found.\n", adr);
      out += buffer;
      desc = nullptr;

      if (adr == 0x11) {
        desc = "SI4721";
      } else if (adr == 0x27) {
        desc = "LCD,PCF8574";
      } else if (adr == 0x38) {
        desc = "AHT20";
      } else if (adr == 0x3C) {
        desc = "SH1106,SSD1306,SSD1309";
      } else if (adr == 0x40) {
        desc = "INA219,INA226) fou";
      } else if (adr == (0x51)) {
        desc = "RTC,PCF8563";
      } else if (adr == (0x5c)) {
        desc = "AM2320";
      } else if (adr == 0x62) {
        desc = "SCD-4x";
      } else if (adr == 0x63) {
        desc = "Radio,SI4730";
      } else if (adr == (0x68)) {
        desc = "RTC,DS1307";
      } else if (adr == 0x77) {
        desc = "BMP280";
      }

      if (desc) {
        out.concat("  (");
        out.concat(desc);
        out.concat(")\n");
      }
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
}  // _scanI2C


/**
 * @brief Activate the DiagElement.
 */
void DiagElement::start() {
  TRACE("start()");
  Element::start();

  // enable I2C scan output using http://nodeding/diag
  _board->server->on("/diag", HTTP_GET, [this]() {
    _board->server->send(200, "text/plain", _scanI2C());
  });


#if defined(ESP8266)
  TRACE("Reset Reason: %s", ESP.getResetReason().c_str());
  TRACE("     Chip-Id: 0x%08X", ESP.getChipId());
#elif defined(ESP32)
  // https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/ResetReason/ResetReason.ino
  TRACE("Reset Reason: %d", rtc_get_reset_reason(0));
#endif

  TRACE(" Free Memory: %d", ESP.getFreeHeap());
  TRACE(" Mac-address: %s", WiFi.macAddress().c_str());

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
