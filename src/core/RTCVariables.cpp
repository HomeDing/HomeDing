/**
 * @file RTCVariables.cpp
 * @author Matthias Hertel (https://www.mathertel.de)
 * 
 * @brief This class implements some functions to access the variables in RTC Memory for ESP8266.
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 * 
 * Changelog: see RTCVariables.h
 */

#include <Arduino.h>
#include <HomeDing.h>


#if defined(ESP8266)
// RTC Memory in ESP8266 is accessible using 32-bit values
#define NETRESET_OFFSET 3 // Length of 1

#define STATE_OFFSET 4
#define STATE_DATA (STATE_OFFSET + 1)
#define STATE_SIZE (200) // state max. length of 200 + 4 bytes of Magic

#elif defined(ESP32)
#endif

/** The magic 4-byte number indicating that a reset has happened before, "RST". Byte 4 is the counter */
#define NETRESET_MAGIC ((uint32_t)0x52535400)
#define NETRESET_MASK ((uint32_t)0xFFFFFF00)
#define NETRESET_VALUE ((uint32_t)0x000000FF)

/** The magic 4-char string "HDS," indicating that a valid state is saved to rtc memory. /reverse byte order */
#define STATE_MAGIC ((uint32_t)0x2c534448)

/**
 * @brief Set the Reset Counter object
 */
int RTCVariables::setResetCounter(int cnt)
{
  // The Reset counter supports a counter in the range 0..255
  // that is stored in the lowest byte of the 32-bit value.
  // The higher 3 bytes are used to check for a valid entry.
  cnt = constrain(cnt, 0, 255);
#if defined(ESP8266)
  uint32_t memData = (NETRESET_MAGIC + cnt);
  ESP.rtcUserMemoryWrite(NETRESET_OFFSET, &memData, sizeof(memData));
#elif defined(ESP32)
  // TODO:ESP32 implementation
#endif
  return (cnt);
};


/**
 * @brief Retrieve the reset counter value.
 */
int RTCVariables::getResetCounter()
{
  int cnt = 0;

#if defined(ESP8266)
  uint32_t memData;
  ESP.rtcUserMemoryRead(NETRESET_OFFSET, &memData, sizeof(memData));
  if ((memData & NETRESET_MASK) == NETRESET_MAGIC) {
    cnt = (memData & 0x000000FF);
  }
#elif defined(ESP32)
  // TODO:ESP32 implementation
#endif
  return (cnt);
};


/**
 * @brief Set the State string.
 */
void RTCVariables::setStateString(String &state)
{

  unsigned int len = state.length();
  if (len < 200) {
#if defined(ESP8266)
    uint32_t memMagic = STATE_MAGIC;
    ESP.rtcUserMemoryWrite(STATE_OFFSET, &memMagic, sizeof(memMagic));
    ESP.rtcUserMemoryWrite(STATE_DATA, (uint32_t *)state.c_str(), len + 1);
#elif defined(ESP32)
  // TODO:ESP32 implementation
#endif
  } // if
}


/**
 * @brief Get the State String object
 */
String RTCVariables::getStateString()
{
  String state;

#if defined(ESP8266)
  uint32_t memMagic;
  ESP.rtcUserMemoryRead(STATE_OFFSET, &memMagic, sizeof(memMagic));
  if (memMagic == STATE_MAGIC) {
    char mem[STATE_SIZE];
    ESP.rtcUserMemoryRead(STATE_DATA, (uint32_t *)mem, sizeof(mem));
    state = mem;
  }
  LOGGER_INFO("readState=\"%s\"", state.c_str());
#elif defined(ESP32)
  // TODO:ESP32 implementation
#endif
  return (state);
};
