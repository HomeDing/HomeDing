// hdProfile.h
//
// Include this file in element.h to add profiling information to elements
// that allows detecting long-running loop() functions of elements.
// Use the DiagElement to display current data in the browser.

#pragma once

#define HD_PROFILE

#if defined(HD_PROFILE)

// structure for collecting Profile data
struct hd_profiledata_t {
  unsigned long totalDuration = 0;
  unsigned long totalCount = 0;
  unsigned long totalMem = 0;
  unsigned long maxDuration = 0;
  unsigned long start;
  unsigned long mem;
};

#define PROFILE_DATA struct hd_profiledata_t profile

#define PROFILE_START(e) \
  { \
    e->profile.start = micros(); \
    e->profile.mem = esp_get_free_heap_size(); \
  }

#define PROFILE_END(e) \
  { \
    unsigned long delta = micros() - e->profile.start; \
    e->profile.totalDuration += delta; \
    e->profile.totalCount++; \
    if (delta > e->profile.maxDuration) e->profile.maxDuration = delta; \
    \
    delta = e->profile.mem - esp_get_free_heap_size(); \
    e->profile.totalMem += delta; \
  }
#else

#define PROFILE_DATA
#define PROFILE_START(obj)
#define PROFILE_END(obj)
#endif
