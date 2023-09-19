
#include "Arduino.h"

#include <lib/gt911.h>
#include <WireUtils.h>

#define GT911_I2CADDR_5D 0x5D
#define GT911_I2CADDR_14 0x14

#define GT911_CONTACT_SIZE 8  // 8 bytes of data per contact

/// Config Registers (0x8047 - 0X80FF)
#define GT911_CONFIG_START 0x8047  // 4 bytes x-low, x-high, y-low, y-high
#define GT911_CONFIG_END 0X80FE
#define GT911_CONFIG_SIZE (uint16_t)(GT911_CONFIG_END - GT911_CONFIG_START + 1)

#define GT911_CONFIG_ALL 0X8100
#define GT911_CONFIG_CHKSUM (uint16_t)0X80FF
#define GT911_CONFIG_FRESH (uint16_t)0X8100


#define GT911_COMMAND (uint16_t)0x8040
#define GT911_ESD_CHECK (uint16_t)0x8041
#define GT911_COMMAND_CHECK (uint16_t)0x8046
#define GT911_RESOLUTION 0x8048  // 4 bytes x-low, x-high, y-low, y-high

/// Data Registers (0x81xx)

#define GT911_TOUCHFLAGS 0X814E
#define GT911_POINTS 0x814F

// Real-time command (Write only)



#define TRACE(...)  // Serial.printf("GT911: " __VA_ARGS__)


GT911::GT911(int _int, int _rst, uint16_t _width, uint16_t _height)
  : pinInt(_int), pinRst(_rst), width(_width), height(_height) {
}

void GT911::init(int address) {
  TRACE("init(0x%02x, rst=%d, int=%d)\n", address, pinRst, pinInt);

  uint8_t regBuffer[2];
  uint16_t configSize = GT911_CONFIG_ALL - GT911_CONFIG_START + 1;
  uint8_t configBuf[configSize];

  addr = address;
  regBuffer[0] = highByte(GT911_CONFIG_START);
  regBuffer[1] = lowByte(GT911_CONFIG_START);

  pinMode(pinRst, OUTPUT);
  if (pinInt < 0) {
    digitalWrite(pinRst, LOW);
    delay(20);
    digitalWrite(pinRst, HIGH);

  } else {
    pinMode(pinInt, OUTPUT);

    // initialization sequence with reset and address selection
    digitalWrite(pinInt, LOW);
    digitalWrite(pinRst, LOW);
    delay(10);
    digitalWrite(pinInt, (address == GT911_I2CADDR_14));
    delay(1);
    digitalWrite(pinRst, HIGH);
    delay(5);
    digitalWrite(pinInt, LOW);
    delay(50);

    // enable interrupt
    pinMode(pinInt, INPUT);
    // attachInterrupt(pinInt, GT911::onInterrupt, RISING);
  }
  delay(50);

#if defined(CONFIG_IDF_TARGET_ESP32S3)
  Wire.setBufferSize(configSize + 4);
#endif

  // read current config.
  WireUtils::txrx(
    addr,
    regBuffer, sizeof(regBuffer),
    configBuf, GT911_CONFIG_START - GT911_CONFIG_END + 1);

  // set resolution
  configBuf[GT911_RESOLUTION - GT911_CONFIG_START + 0] = lowByte(width);
  configBuf[GT911_RESOLUTION - GT911_CONFIG_START + 1] = highByte(width);
  configBuf[GT911_RESOLUTION - GT911_CONFIG_START + 2] = lowByte(height);
  configBuf[GT911_RESOLUTION - GT911_CONFIG_START + 3] = highByte(height);

  // calc checksum
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < GT911_CONFIG_SIZE; i++) {
    checksum += configBuf[i];
  }
  checksum = (~checksum) + 1;
  configBuf[GT911_CONFIG_CHKSUM - GT911_CONFIG_START] = checksum;
  configBuf[GT911_CONFIG_FRESH - GT911_CONFIG_START] = 1;

  Wire.beginTransmission(addr);
  Wire.write(regBuffer, sizeof(regBuffer));
  Wire.write(configBuf, GT911_CONFIG_START - GT911_CONFIG_ALL + 1);
  Wire.endTransmission();
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
    contacts = pointInfo & 0xF;
    TRACE(" contacts: %d\n", contacts);
  }

  if (contacts > GT911_MAX_CONTACTS) {
    init(addr == GT911_I2CADDR_14);
    return (0);

  } else if (contacts > 0) {
    // read touch points

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

      // Serial.printf("read: %d: %d/%d\n", i, points[i].x, points[i].y);
    }
  }
  if (pointInfo != 0x00) {
    WireUtils::write(addr, highByte(GT911_TOUCHFLAGS), lowByte(GT911_TOUCHFLAGS), 0);
  }
  return (contacts);
}  // getTouchPoints()
