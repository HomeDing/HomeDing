// hdtrace.h
// Include this file

#pragma once

#define HD_TRACE

#if defined(HD_TRACE)

struct hd_functime_t {
  unsigned long totalDuration = 0;
  unsigned long totalCount = 0;
  unsigned long start;
};

#define TRACE_DATA struct hd_functime_t functrace

#define TRACE_START(e) \
  { e->functrace.start = micros(); }

#define TRACE_END(e) \
  { \
    e->functrace.totalDuration += (micros() - e->functrace.start); \
    e->functrace.totalCount++; \
  }

#define TRACE_TIMEPRINT(e, topic) \
  { Serial.printf("%-17s %5ldµsec (%4ld)\n", topic, (e->functrace.totalDuration / e->functrace.totalCount), e->functrace.totalCount); }

#define TRACE_TIMEPRINTBUF(buffer, e, topic) \
  { sprintf(buffer, " %-19s %7ld usec (%4ld)\n", topic, (e->functrace.totalDuration / e->functrace.totalCount), e->functrace.totalCount); }

#else

#define TRACE_DATA
#define TRACE_START(obj)
#define TRACE_END(obj)
#define TRACE_TIMEPRINT(topic, id, min)
#endif
