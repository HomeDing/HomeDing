/**
 * @file WireUtils.cpp
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
 * Changelog: see WireUtils.h
 */


#include "WireUtils.h"
#include "HardwareSerial.h"

// activate to get some debug output.
// #define WIREDUMP

/** Helper function to inspect a data buffer by dumping in hexadecimal format. */
void WireUtils::dumpBuffer(uint8_t *data, uint8_t len)
{
  if (data) {
    while (len > 0) {
      Serial.printf(" %02x", *data++);
      len--;
    } // while
    Serial.println();
  } // if
} // dumpBuffer()


/** check for a device on address */
bool WireUtils::exists(uint8_t address)
{
  Wire.beginTransmission(address);
  uint8_t err = Wire.endTransmission();
  return (err == 0);
} // exists()


/** read one register value */
uint8_t WireUtils::readRegister(uint8_t address, uint8_t reg)
{
  uint8_t data;
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(address, (uint8_t)1);
  data = Wire.read();
  return (data);
} // read()


/** read a sequence of register values into a buffer.
 * @return number of register values read.
 */
uint8_t WireUtils::readBuffer(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len, int wait)
{
  uint8_t *d = data;
  uint8_t done = 0;

  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission();

  delay(wait);

  Wire.requestFrom(address, len);
  while (Wire.available() && (done < len)) {
    *d = Wire.read();
    done++;
    d++;
  }

#ifdef WIREDUMP
  Serial.printf("i2c read 0x%02x:", reg);
  dumpBuffer(data, len);
#endif

  return (done);
} // read()


uint8_t WireUtils::write(uint8_t address, uint8_t reg) {
  return (write(address, reg, nullptr, 0));
}


uint8_t WireUtils::write(uint8_t address, uint8_t reg, uint8_t data)
{
  return (write(address, reg, &data, 1));
} // write()


// read sequence of bytes to buffer
uint8_t WireUtils::write(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len)
{
#ifdef WIREDUMP
  Serial.printf("i2c writ 0x%02x:", reg);
  dumpBuffer(data, len);
#endif

  Wire.beginTransmission(address);
  Wire.write(reg);
  while (len) {
    Wire.write(*data);
    len--;
    data++;
  }
  return (Wire.endTransmission());
} // write()


/** read a sequence of values from i2c device into a buffer.
 * @return number of register values read.
 */
uint8_t WireUtils::request(uint8_t address, uint8_t *data, uint8_t len)
{
  uint8_t *d = data;
  uint8_t done = Wire.requestFrom(address, len);
  for (int n= 0; n < done; n++) {
    *d++ = Wire.read();
  }  

#ifdef WIREDUMP
  Serial.printf("i2c request:");
  dumpBuffer(data, done);
#endif

  return (done);
} // read()


// End.
