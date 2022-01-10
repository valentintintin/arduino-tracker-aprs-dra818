#ifndef ARDUINO_TRACKER_APRS_DRA818_UTILS_H
#define ARDUINO_TRACKER_APRS_DRA818_UTILS_H

#define DPRINT(...) Serial.print(__VA_ARGS__)
#define DPRINTLN(...) Serial.println(__VA_ARGS__)

#ifdef TEST
#define IS_TEST_MODE true
#else
#define IS_TEST_MODE false
#endif

void blink(byte nb);

#endif //ARDUINO_TRACKER_APRS_DRA818_UTILS_H
