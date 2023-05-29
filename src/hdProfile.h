// hdProfile.h
// 
// Include this file in element.h to add profiling information to elements
// that allows detectonmg long-running lopp() functions of elements.
// Use the DiagElement to display current data in the browser.

#pragma once

#define HD_PROFILE

#if defined(HD_PROFILE)

// structure for collecting Profile data
struct hd_profiledata_t {
  unsigned long totalDuration = 0;
  unsigned long totalCount = 0;
  unsigned long start;
};

#define PROFILE_DATA struct hd_profiledata_t profile

#define PROFILE_START(e) \
  { e->profile.start = micros(); }

#define PROFILE_END(e) \
  { \
    e->profile.totalDuration += (micros() - e->profile.start); \
    e->profile.totalCount++; \
  }

#define PROFILE_TIMEPRINT(e, topic) \
  { Serial.printf("%-17s %5ldµsec (%4ld)\n", topic, (e->profile.totalDuration / e->profile.totalCount), e->profile.totalCount); }

#define PROFILE_TIMEPRINTBUF(buffer, e, topic) \
  { sprintf(buffer, " %-19s %7ld usec (%4ld)\n", topic, (e->profile.totalDuration / e->profile.totalCount), e->profile.totalCount); }

#else

#define PROFILE_DATA
#define PROFILE_START(obj)
#define PROFILE_END(obj)
#define PROFILE_TIMEPRINT(topic, id, min)
#endif
