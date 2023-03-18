/**
 * @file SDMMCElement.cpp
 * @brief Supporting a mmc sd card reader as file storage and Element for the HomeDing Library.
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see SDMMCElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#if defined(ESP32)

#include "SDMMCElement.h"

#include "SD_MMC.h"
#include <hdfs.h>

#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new SDMMCElement
 * @return SDMMCElement* created element
 */
Element *SDMMCElement::create() {
  return (new SDMMCElement());
}  // create()


/* ===== Element functions ===== */

SDMMCElement::SDMMCElement() {
  // adjust startupMode when Network (default) is not applicable.
  startupMode = Element_StartupMode::System;
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool SDMMCElement::set(const char *name, const char *value) {
  TRACE("set %s=%s", name, value);
  bool ret = true;

  if (Element::set(name, value)) {
    // done.

  } else if (_stricmp(name, "mmcd0") == 0) {
    _mmc_d0_pin = _atopin(value);

  } else if (_stricmp(name, "mmcd1") == 0) {
    _mmc_d1_pin = _atopin(value);

  } else if (_stricmp(name, "mmcd2") == 0) {
    _mmc_d2_pin = _atopin(value);

  } else if (_stricmp(name, "mmcd3") == 0) {
    _mmc_d3_pin = _atopin(value);

  } else if (_stricmp(name, "mmcclk") == 0) {
    _mmc_clk_pin = _atopin(value);

  } else if (_stricmp(name, "mmccmd") == 0) {
    _mmc_cmd_pin = _atopin(value);

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


/**
 * @brief Activate the SDMMCElement.
 */
void SDMMCElement::start() {
  TRACE("start()");

  // Verify parameters

  // for ESP-S3 the pins for SD_MMC can be defined.
  // other boards ignore the settings, GPIO pins have to be used as defined by espressif
#if ARDUINO_ESP32S3_DEV
  if ((_mmc_clk_pin >= 0) && (_mmc_cmd_pin >= 0) && (_mmc_d0_pin >= 0)) {
    SD_MMC.setPins(_mmc_clk_pin, _mmc_cmd_pin, _mmc_d0_pin, _mmc_d1_pin, _mmc_d2_pin, _mmc_d3_pin);
  }
#endif

  if (!SD_MMC.begin()) {
    TRACE("Card Mount Failed");

  } else {
    Element::start();

    // mount the sd filesystem
    HomeDingFS::sdFS = &SD_MMC;

    uint8_t cardType = SD_MMC.cardType();
    TRACE("Card Type: %d", cardType);

    uint32_t cardSize = SD_MMC.cardSize() / (1024 * 1024);

    TRACE("SD_MMC Card Size: %d MByte", SD_MMC.cardSize() / (1024 * 1024));
    TRACE(" Total: %d MByte", SD_MMC.totalBytes() / (1024 * 1024));
    TRACE(" Used : %d MByte", SD_MMC.usedBytes() / (1024 * 1024));
  }

  _board->server->serveStatic(SD_MOUNTNAME, SD_MMC, "/", nullptr);
}  // start()


/* ===== Register the Element ===== */

// As long as the Element is project specific or is a element always used
// the registration is placed here without using a register #define.

// When transferred to the HomeDing library a #define like the
// HOMEDING_INCLUDE_XXX should be used to allow the sketch to select the
// available Elements. See <HomeDing.h> the move these lines to SDMMCElement.h:

// #ifdef HOMEDING_REGISTER
// Register the SDMMCElement onto the ElementRegistry.
bool SDMMCElement::registered =
  ElementRegistry::registerElement("sdmmc", SDMMCElement::create);
// #endif

#endif

// End
