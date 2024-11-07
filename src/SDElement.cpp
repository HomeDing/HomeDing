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

#if defined(ESP8266)
#include <SDFS.h>
#endif

#include <hdfs.h>

#define TRACE(...) // LOGGER_ETRACE(__VA_ARGS__)

/* ===== Static factory function ===== */

/**
 * @brief static factory function to create a new SDElement
 * @return SDElement* created element
 */
Element *SDElement::create() {
  SDElement *e =new SDElement();
  e->category = CATEGORY::Standard; // no polling
  return (e);
}  // create()


/* ===== Element functions ===== */

SDElement::SDElement() {
  // adjust startupMode when Network (default) is not applicable.
  startupMode = Element::STARTUPMODE::System;
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
  if (_csPin < 0) {
    LOGGER_EERR("cspin parameter is missing.");
  }

#if defined(ESP8266)
  // using the FS compartible SDFS class (not SD)
  SDFSConfig fileSystemConfig = SDFSConfig();
  fileSystemConfig.setCSPin(_csPin);
  fileSystemConfig.setAutoFormat(false);
  SDFS.setConfig(fileSystemConfig);

  // SPI.pins(D5, D6, D7, D4);

  if (!SDFS.begin()) {
    TRACE("Card Mount Failed");

  } else {
    Element::start();

    // mount the sd filesystem
    HomeDingFS::sdFS = &SDFS;

    FSInfo64 fsi;
    SDFS.info64(fsi);
    TRACE(" Total: %ld MByte", fsi.totalBytes / (1024 * 1024));
    TRACE(" Used : %ld MByte", fsi.usedBytes / (1024 * 1024));
  }


#elif defined(ESP32)

  // restart SPI
  SPI.end();  // maybe corrupted by previous SPI usages (display configuration)
  SPI.begin(_board->spiCLK, _board->spiMISO, _board->spiMOSI);

  if (!SD.begin(_csPin, SPI)) {
    TRACE("Card Mount Failed");

  } else {
    Element::start();

    // mount the sd filesystem
    HomeDingFS::sdFS = &SD;

    TRACE("Card Type: %d", SD.cardType());
    TRACE("SD Card Size: %llu MByte", SD.cardSize() / (1024 * 1024));
    TRACE(" Total: %llu MByte", SD.totalBytes() / (1024 * 1024));
    TRACE(" Used : %llu MByte", SD.usedBytes() / (1024 * 1024));
  }
#endif

  if (active) {
    TRACE("serve..");
    _board->server->serveStatic(HDFS_SD_MOUNTNAME, *HomeDingFS::sdFS, "/", nullptr);
  }

}  // start()

// End
