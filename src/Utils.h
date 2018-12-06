#ifndef ARDUINO_TRACKER_APRS_DRA818_UTILS_H
#define ARDUINO_TRACKER_APRS_DRA818_UTILS_H

#define DEBUG
#ifdef DEBUG
#define DPRINT(...) Serial.print(__VA_ARGS__)
#define DPRINTLN(...) \
  Serial.println(__VA_ARGS__)
#else
#define DPRINT(...)
#define DPRINTLN(...)
#endif

void blink(byte nb);

#endif //ARDUINO_TRACKER_APRS_DRA818_UTILS_H
