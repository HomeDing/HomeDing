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

#include <Arduino.h>
#include <ESP8266mDNS.h>

#include <Board.h>
#include <Element.h>

#include <HomeDing.h>

#include "DiagElement.h"


/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new DiagElement
 * @return DiagElement* created element
 */
Element *DiagElement::create()
{
  return (new DiagElement());
} // create()


/* ===== Element functions ===== */

DiagElement::DiagElement()
{
  startupMode = Element_StartupMode::System;
}

void DiagElement::init(Board *board)
{
  Element::init(board);

  // startup before network
  startupMode = Element_StartupMode::System;
} // init()


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool DiagElement::set(const char *name, const char *value)
{
  bool ret = true;

  if (_stricmp(name, "heap") == 0) {
    // log some heap information. using http://nodeding/$board/diag/0?heap=1
    LOGGER_ETRACE("===== HEAP =====");
    uint32_t free;
    uint16_t max;
    uint8_t frag;
    ESP.getHeapStats(&free, &max, &frag);
    LOGGER_EINFO("heap free: %5d - max: %5d - frag: %3d%%", free, max, frag);


  } else if (_stricmp(name, "rtcmem") == 0) {
    // log some heap information. using http://nodeding/$board/diag/0?rtcmem=1
    // dump rtc Memory
    LOGGER_ETRACE("===== RTCMEM =====");
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
      LOGGER_ETRACE("  %04x: %s%s", adr, bytes.c_str(), chars.c_str());
    } // for

  } else if (_stricmp(name, "mdns") == 0) {
    // log some heap information. using http://nodeding/$board/diag/0?mdns=1
    LOGGER_ETRACE("===== mdns =====");
    Serial.flush();

    // add mDNS service discovery feature
    // see http://www.dns-sd.org/
    // https://tools.ietf.org/html/rfc6762
    // https://tools.ietf.org/html/rfc6763
    MDNSResponder::hMDNSService serv = MDNS.addService(0, "http", "tcp", 80);
    MDNS.addServiceTxt(serv, "path", "/");

    LOGGER_ETRACE("done.");

  } else {
    ret = Element::set(name, value);
  } // if

  return (ret);
} // set()


/**
 * @brief Activate the DelementiagElement.
 */
void DiagElement::start()
{
  delay(2000);
  LOGGER_ETRACE("start()");
  Element::start();

  LOGGER_ETRACE("Reset Reason: %s", ESP.getResetReason().c_str());
  LOGGER_ETRACE(" Free Memory: %d", ESP.getFreeHeap());
  LOGGER_ETRACE("     Chip-Id: 0x%08X", ESP.getChipId());
  LOGGER_ETRACE(" Mac-address: %s", WiFi.macAddress().c_str());

  // ===== scan the the I2C bus and report found devices =====
  LOGGER_ETRACE("Scan i2c (sda=%d, scl=%d)...", _board->I2cSda, _board->I2cScl);

  int error, adr;
  int num;

  num = 0;
  for (adr = 1; adr < 127; adr++) {
    // The i2c scanner uses the return value of Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(adr);
    error = Wire.endTransmission();

    if (error == 0) {
      LOGGER_ETRACE(" 0x%02x found.", adr);
      num++;
    } else if (error == 4) {
      LOGGER_ETRACE(" 0x%02x error.", adr);
    } // if
    yield();
  } // for
  LOGGER_ETRACE(" %2d devices found.", num);

} // start()


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
