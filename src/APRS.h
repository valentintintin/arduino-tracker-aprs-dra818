#ifndef ARDUINO_TRACKER_APRS_DRA818_APRS_H
#define ARDUINO_TRACKER_APRS_DRA818_APRS_H

#include <Arduino.h>
#include "DRA.h"
#include "GPS.h"

class APRS {
public:
    APRS(DRA *dra, GPS *gps, uint8_t txPin);

    void init(char *call, uint8_t callId, char *toCall, uint8_t toCallId, char *relays);

    bool sendIfPossible(bool forceGps = false, bool forceTx = false);

    void setComment(String comment);

private:
    DRA *dra = nullptr;
    GPS *gps = nullptr;

    uint8_t lastSpeed = 0;
    uint64_t lastTx = 0;
    uint8_t txPin = 0;

    String packetBuffer;
    String comment;

    long readVccAtmega();
    uint16_t getTimeSecondsForGivenSpeed();

    float convertDegMin(float decDeg);
    void stringPadding(int number, byte width, String *dest);
    void stringPaddingf(float number, byte width, String *dest);

    void buildPacket();
    bool txToRadio();
    bool sendPosition();
};


#endif //ARDUINO_TRACKER_APRS_DRA818_APRS_H
