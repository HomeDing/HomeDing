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
 * 16.06.2023 write/read
 */


#pragma once

#include <Arduino.h>
#include <Wire.h>

/** extract signed 16 bit value from byte buffer */
#define WU_S16(data, offset) (int16_t)(data[offset + 1] << 8 | data[offset])

/** extract unsigned 16 bit value from byte buffer */
#define WU_U16(data, offset) (uint16_t)(data[offset + 1] << 8 | data[offset])


class WireUtils {
public:
  /** Helper function to inspect a data buffer by dumping in hexadecimal format. */
  static void dumpBuffer(uint8_t *data, uint8_t len);

  /** check for a device on address */
  static bool exists(uint8_t address);

  /** read a sequence of bytes from device into a buffer.
   * @return number of register values read.
   */
  static uint8_t readBuffer(uint8_t address, uint8_t *data, uint8_t len);

  /** write a sequence of bytes from buffer to device.
   * @return 0 or error code
   */
  static uint8_t writeBuffer(uint8_t address, uint8_t *data, uint8_t len);




  /** read one byte register value */
  static uint8_t readRegister(uint8_t address, uint8_t reg);

  /** read a sequence of register values into a buffer.
   * @return number of register values read.
   */
  static uint8_t readBuffer(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len);

  /// send a single byte to i2c address
  static uint8_t write(uint8_t address, uint8_t reg);

  /// send 2 bytes to i2c address
  static uint8_t write(uint8_t address, uint8_t data1, uint8_t data2);

  /// send 3 bytes to i2c address
  static uint8_t write(uint8_t address, uint8_t data1, uint8_t data2, uint8_t data3);

  /// Send and receive sequence with buffers
  static uint8_t txrx(
    uint8_t address, 
    uint8_t *txBuffer, uint8_t txLen,
    uint8_t *rxBuffer, uint8_t rxLen);
};
