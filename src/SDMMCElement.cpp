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

#if defined(ESP32) && (! CONFIG_IDF_TARGET_ESP32C3)

#include "SDMMCElement.h"

#include <SD_MMC.h>
#include <hdfs.h>

#if !defined(TRACE)
#define TRACE(...)  // LOGGER_ETRACE(__VA_ARGS__)
#endif

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new SDMMCElement
 * @return SDMMCElement* created element
 */
Element *SDMMCElement::create() {
  SDMMCElement *e =new SDMMCElement();
  e->category = CATEGORY::Standard; // no polling
  return (e);
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

#if defined(CONFIG_IDF_TARGET_ESP32S3)
  // support of pin mapping for MMC cards
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
#endif

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

#if defined(CONFIG_IDF_TARGET_ESP32S3)
  // for ESP-S3 the pins for SD_MMC can be defined.
  if ((_mmc_clk_pin >= 0) && (_mmc_cmd_pin >= 0) && (_mmc_d0_pin >= 0)) {
    SD_MMC.setPins(_mmc_clk_pin, _mmc_cmd_pin, _mmc_d0_pin, _mmc_d1_pin, _mmc_d2_pin, _mmc_d3_pin);
  }
#else
  // other boards ignore the settings, GPIO pins have to be used as defined by espressif
  // https://github.com/lewisxhe/TTGO-Time-Music-Box/blob/master/main/src/main.c
  // gpio_set_pull_mode(GPIO_NUM_15, GPIO_PULLUP_ONLY);
  // gpio_set_pull_mode(GPIO_NUM_2, GPIO_PULLUP_ONLY);
  // gpio_set_pull_mode(GPIO_NUM_4, GPIO_PULLUP_ONLY);
  // gpio_set_pull_mode(GPIO_NUM_12, GPIO_PULLUP_ONLY);
  // gpio_set_pull_mode(GPIO_NUM_13, GPIO_PULLUP_ONLY);
#endif

  SD_MMC.end();
  if (!SD_MMC.begin()) {
    TRACE("Card Mount Failed");

  } else {
    Element::start();

    // mount the sd filesystem
    HomeDingFS::sdFS = &SD_MMC;

    TRACE("Card Type: %d", SD_MMC.cardType());
    TRACE("SD_MMC Card Size: %d MByte", SD_MMC.cardSize() / (1024 * 1024));
    TRACE(" Total: %d MByte", SD_MMC.totalBytes() / (1024 * 1024));
    TRACE(" Used : %d MByte", SD_MMC.usedBytes() / (1024 * 1024));
  }

  _board->server->serveStatic(SD_MOUNTNAME, SD_MMC, "/", nullptr);
}  // start()

#endif

// End
