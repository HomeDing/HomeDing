/**
 * @file SDElement.cpp
 * @brief Supporting a sd card reader as file storage and Element for the HomeDing Library.
 * @author Matthias Hertel, https://www.mathertel.de
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * More information on https://www.mathertel.de/Arduino
 *
 * Changelog:see SDElement.h
 */

#include <Arduino.h>
#include <HomeDing.h>

#include "SDElement.h"

#include "SPI.h"
#include <SD.h>
#include <SDFS.h>

#include <hdfs.h>

#define TRACE(...) LOGGER_ETRACE(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new SDElement
 * @return SDElement* created element
 */
Element *SDElement::create() {
  return (new SDElement());
}  // create()


/* ===== Element functions ===== */

SDElement::SDElement() {
  // adjust startupMode when Network (default) is not applicable.
  startupMode = Element_StartupMode::System;
}


/**
 * @brief Set a parameter or property to a new value or start an action.
 */
bool SDElement::set(const char *name, const char *value) {
  TRACE("set %s=%s", name, value);
  bool ret = true;

  if (Element::set(name, value)) {
    // done.

  } else if (_stricmp(name, "cspin") == 0) {
    _csPin = _atopin(value);

  } else {
    ret = false;
  }  // if

  return (ret);
}  // set()


/**
 * @brief Activate the SDElement.
 */
void SDElement::start() {
  TRACE("start()");

  // Verify parameters
  // _csPin >= 0;

  // SD.end();
  SDFSConfig fileSystemConfig = SDFSConfig();
  fileSystemConfig.setCSPin(D4);
  fileSystemConfig.setAutoFormat(false);

  SDFS.setConfig(fileSystemConfig);

  // SPI.pins(D5, D6, D7, D4);

  if (!SDFS.begin()) {
    TRACE("Card Mount Failed");

  } else {
    Element::start();

    // mount the sd filesystem
    HomeDingFS::sdFS = &SDFS;

    // uint8_t cardType = SD.cardType();
    // TRACE("Card Type: %d", cardType);

    // uint32_t cardSize = SD.size64() / (1024 * 1024);

    // TRACE("SD Card Size: %d MByte", SD.size64() / (1024 * 1024));
    FSInfo64 fsi;
    SDFS.info64(fsi);

    TRACE(" Total: %d MByte", fsi.totalBytes / (1024 * 1024));
    TRACE(" Used : %d MByte", fsi.usedBytes / (1024 * 1024));
  }

  _board->server->serveStatic(HDFS_SD_MOUNTNAME, SDFS, "/", nullptr);
}  // start()

// End
