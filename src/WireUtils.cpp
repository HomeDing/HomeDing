/**
 * @file WireUtils.cpp
 *
 * @brief Wire Utils are some helpful methods and makros to use the i2c bus
 * through the Wire library,
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
 * Changelog: see WireUtils.h
 */


#include <Arduino.h>
#include <WireUtils.h>

// activate to get some debug output.
// #define WIREDUMP

/** Helper function to inspect a data buffer by dumping in hexadecimal format. */
void WireUtils::dumpBuffer(uint8_t *data, uint8_t len) {
#ifdef WIREDUMP
  if (data) {
    while (len > 0) {
      Serial.printf(" %02x", *data++);
      len--;
    }  // while
    Serial.println();
  }  // if
#endif
}  // dumpBuffer()


/** check for a device on address */
bool WireUtils::exists(uint8_t address) {
  Wire.beginTransmission(address);
  uint8_t err = Wire.endTransmission();
  return (err == 0);
}  // exists()


/** read a sequence of register values into a buffer.
 * @return number of register values read.
 */
uint8_t WireUtils::readBuffer(uint8_t address, uint8_t *data, uint8_t len) {
  uint8_t *d = data;

  uint8_t done = Wire.requestFrom(address, len);
  for (int n = 0; n < done; n++) {
    *d++ = (uint8_t)Wire.read();
  }

#ifdef WIREDUMP
  Serial.printf("i2c <read: ");
  dumpBuffer(data, len);
#endif

  return (done);
}  // read()


// write a sequence of bytes from buffer
uint8_t WireUtils::writeBuffer(uint8_t address, uint8_t *data, uint8_t len) {
#ifdef WIREDUMP
  Serial.printf("i2c >writ: ");
  dumpBuffer(data, len);
#endif

  Wire.beginTransmission(address);
  while (len) {
    Wire.write(*data++);
    len--;
  }
  return (Wire.endTransmission());
}  // writeBuffer()


// === some useful functions

// write one byte to device
uint8_t WireUtils::write(uint8_t address, uint8_t reg) {
  uint8_t tmp[1] = { reg };
  return (writeBuffer(address, tmp, 1));
}  // write()


// write two bytes to device
uint8_t WireUtils::write(uint8_t address, uint8_t reg, uint8_t data) {
  uint8_t tmp[2] = { reg, data };
  return (writeBuffer(address, tmp, 2));
}  // write()


/** read one register value */
uint8_t WireUtils::readRegister(uint8_t address, uint8_t reg) {
  uint8_t data;
  write(address, reg);
  readBuffer(address, &data, 1);
  return (data);
}  // readRegister()



/** read a sequence of register values into a buffer.
 * @return number of register values read.
 */
uint8_t WireUtils::readBuffer(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len) {
  write(address, reg);
  return (readBuffer(address, data, len));
}  // readBuffer()

// End.
