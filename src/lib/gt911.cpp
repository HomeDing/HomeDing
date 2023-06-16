
#include "Arduino.h"

#include <lib/gt911.h>
#include <WireUtils.h>

#define GT911_I2CADDR_5D 0x5D
#define GT911_I2CADDR_14 0x14

#define GT911_CONTACT_SIZE 8  // 8 bytes of data per contact

/// Config Registers (0x8047 - 0X80FF)
#define GT911_CONFIG 0x8047      // 4 bytes x-low, x-high, y-low, y-high
#define GT911_RESOLUTION 0x8048  // 4 bytes x-low, x-high, y-low, y-high

#define GT911_CONFIG_CHKSUM (uint16_t)0X80FF
#define GT911_CONFIG_FRESH (uint16_t)0X8100


/// Register Address of the points


#define GT911_TOUCHFLAGS 0X814E
#define GT911_POINTS 0x814F


GT911::GT911(uint8_t _int, uint8_t _rst, uint16_t _width, uint16_t _height)
  : pinInt(_int), pinRst(_rst), width(_width), height(_height) {
}

void GT911::init(bool useAdr2) {
  uint8_t regBuffer[2];
  Serial.printf("init()\n");

  addr = useAdr2 ? GT911_I2CADDR_14 : GT911_I2CADDR_5D;

  pinMode(pinRst, OUTPUT);
  pinMode(pinInt, OUTPUT);

  // initialization sequence with reset and adress selection
  digitalWrite(pinInt, LOW);
  digitalWrite(pinRst, LOW);
  delay(10);
  digitalWrite(pinInt, useAdr2);
  delay(1);
  digitalWrite(pinRst, HIGH);
  delay(5);
  digitalWrite(pinInt, LOW);
  delay(50);

  // enable interrupt
  pinMode(pinInt, INPUT);
  // attachInterrupt(pinInt, GT911::onInterrupt, RISING);
  delay(50);

  // read current config.
  regBuffer[0] = highByte(GT911_CONFIG);
  regBuffer[1] = lowByte(GT911_CONFIG);
  WireUtils::txrx(
    addr,
    regBuffer, sizeof(regBuffer),
    configBuf, GT911_CONFIG_SIZE);

  // set resolution
  configBuf[GT911_RESOLUTION - GT911_CONFIG + 0] = lowByte(width);
  configBuf[GT911_RESOLUTION - GT911_CONFIG + 1] = highByte(width);
  configBuf[GT911_RESOLUTION - GT911_CONFIG + 2] = lowByte(height);
  configBuf[GT911_RESOLUTION - GT911_CONFIG + 3] = highByte(height);

  regBuffer[0] = highByte(GT911_RESOLUTION);
  regBuffer[1] = lowByte(GT911_RESOLUTION);
  WireUtils::txrx(
    addr,
    regBuffer, sizeof(regBuffer),
    &configBuf[GT911_RESOLUTION - GT911_CONFIG], 4);

  uint8_t checksum = 0;
  for (uint8_t i = 0; i < GT911_CONFIG_SIZE; i++) {
    checksum += configBuf[i];
  }
  configBuf[GT911_CONFIG_CHKSUM - GT911_CONFIG] = (~checksum) + 1;

  WireUtils::write(addr, highByte(GT911_CONFIG_CHKSUM), lowByte(GT911_CONFIG_CHKSUM), configBuf[GT911_CONFIG_CHKSUM - GT911_CONFIG]);
  WireUtils::write(addr, highByte(GT911_CONFIG_FRESH), lowByte(GT911_CONFIG_FRESH), 1);
}

// void ARDUINO_ISR_ATTR GT911::onInterrupt() {
//   read();
//   GT911::onRead();
// }

void GT911::setRotation(int rotation) {
  _rotation = (rotation / 90) % 4;
}

// void GT911::setOnRead(void (*isr)()) {
//   onRead = isr;
// }

uint8_t GT911::getTouchPoints(GDTpoint_t *points) {
  // Serial.println("GT911::getTouchPoints");
  uint8_t regBuffer[2];
  uint8_t rawData[GT911_MAX_CONTACTS * GT911_CONTACT_SIZE];
  uint8_t contacts = 0;
  uint8_t pointInfo = 0x34;

  // read flags only to keep it short as most time there are no touch
  regBuffer[0] = highByte(GT911_TOUCHFLAGS);
  regBuffer[1] = lowByte(GT911_TOUCHFLAGS);
  WireUtils::txrx(
    addr,
    regBuffer, sizeof(regBuffer),
    rawData, 1);
  pointInfo = rawData[0];
  // Serial.printf(" pointInfo: 0x%02x\n", pointInfo);

  if (pointInfo & 0x80) {
    // read touch points
    contacts = pointInfo & 0xF;
    // Serial.printf(" contacts: %d\n", contacts);

    if (contacts > GT911_MAX_CONTACTS) {
      init(addr == GT911_I2CADDR_14);
      return (0);
    }

    memset(points, 0, sizeof(GDTpoint_t) * GT911_MAX_CONTACTS);

    // read existing contacts
    regBuffer[0] = highByte(GT911_POINTS);
    regBuffer[1] = lowByte(GT911_POINTS);
    WireUtils::txrx(
      addr,
      regBuffer, sizeof(regBuffer),
      rawData, GT911_CONTACT_SIZE * contacts);

    for (uint8_t i = 0; i < contacts; i++) {
      uint8_t *rawPoint = &rawData[GT911_CONTACT_SIZE * i];

      points[i].trackId = rawPoint[0];
      uint16_t x = rawPoint[1] + (rawPoint[2] << 8);
      uint16_t y = rawPoint[3] + (rawPoint[4] << 8);
      points[i].area = rawPoint[5] + (rawPoint[6] << 8);

      uint16_t temp;
      if (_rotation == 0) {
        // no turn required

      } else if (_rotation == 1) {
        // 90° to the right
        uint16_t temp = x;
        x = y;
        y = height - temp;

      } else if (_rotation == 2) {
        // 180°
        x = width - x;
        y = height - y;

      } else if (_rotation == 3) {
        // 270°
        uint16_t temp = x;
        x = width - y;
        y = temp;
      }
      points[i].x = x;
      points[i].y = y;

      // Serial.printf("read: %d %d/%d\n", i, points[i].x, points[i].y);
    }
  }
  if (pointInfo != 0x00) {
    WireUtils::write(addr, highByte(GT911_TOUCHFLAGS), lowByte(GT911_TOUCHFLAGS), 0);
  }
  return (contacts);
}  // getTouchPoints()

