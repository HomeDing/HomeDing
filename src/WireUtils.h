/**
 * @file WireUtils.h
 *
 * @brief Wire Utils are some helpful methods and makros to use the i2c bus through the Wire library,
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
 * 05.03.2020 created.
 */


#ifndef WIREUTILS_H
#define WIREUTILS_H


#include <Wire.h>

#define WU_S16(data, offset) (int16_t)(data[offset + 1] << 8 | data[offset])
#define WU_U16(data, offset) (uint16_t)(data[offset + 1] << 8 | data[offset])


class WireUtils
{
public:
  static void dump(uint8_t address, uint8_t reg, uint8_t len);

  static uint8_t test(uint8_t address);

  static uint8_t read(uint8_t address, uint8_t reg);
  static uint8_t read(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len);

  static void write(uint8_t address, uint8_t reg, uint8_t data);
  static void write(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len);

};

#endif