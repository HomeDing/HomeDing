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

void WireUtils::dump(uint8_t address, uint8_t reg, uint8_t len)
{
  uint8_t data;

  Serial.printf("i2c dump 0x%02x:", reg);

  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(address, len);
  while (Wire.available() && (len > 0)) {
    data = Wire.read();
    Serial.printf(" %02x", data);
    len--;
  }
  if (len > 0) {
    Serial.printf(" no more.\n");
  } else {
    Serial.printf(".\n");
  }
}


uint8_t WireUtils::test(uint8_t address)
{
  Wire.beginTransmission(address);
  return (Wire.endTransmission());
}

// read single byte
uint8_t WireUtils::read(uint8_t address, uint8_t reg)
{
  uint8_t data;
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(address, (uint8_t)1);
  data = Wire.read();
  return (data);
} // read()


// read sequence of bytes to buffer
uint8_t WireUtils::read(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len)
{
  uint8_t done = 0;

  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(address, len);
  while (Wire.available() && (done < len)) {
    *data = Wire.read();
    done++;
    data++;
  }
  return (done);
} // read()


void WireUtils::write(uint8_t address, uint8_t reg, uint8_t data)
{
  Wire.beginTransmission(address); // start transmission to device
  Wire.write(reg); // send register address
  Wire.write(data); // send value to write
  Wire.endTransmission(); // end transmission
}


// read sequence of bytes to buffer
void WireUtils::write(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len)
{
  uint8_t done = 0;

  Wire.beginTransmission(address);
  Wire.write(reg);
  while (len) {
    Wire.write(*data);
    len--;
    data++;
  }
  Wire.endTransmission();
} // read()


// End.
