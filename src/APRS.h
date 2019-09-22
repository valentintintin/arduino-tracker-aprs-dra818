#ifndef ARDUINO_TRACKER_APRS_DRA818_APRS_H
#define ARDUINO_TRACKER_APRS_DRA818_APRS_H

#include <Arduino.h>
#include "DRA.h"
#include "GPS.h"

class APRS {
public:
    APRS(DRA *dra, GPS *gps, uint16_t secondBetweenTx, uint8_t speedDeltaTx, uint16_t locationMeterDeltaTx, uint8_t txPin);

    void init(char *call, uint8_t callId, char *toCall, uint8_t toCallId, char *relays);

    bool loop(bool test = false);

    void setTimeBetweenTx(uint16_t timeBetweenTx);

    void setSpeedDeltaTx(uint8_t speedDeltaTx);

    void setLocationDeltaTx(uint16_t localtionDeltaTx);

    void setComment(String comment);

    bool txToRadio(String packet);

    bool sendPosition();

private:
    DRA *dra = nullptr;
    GPS *gps = nullptr;

    uint64_t lastTx = 0;
    double lastSpeed = 0;
    double lastLat = 0;
    double lastLng = 0;


    uint8_t txPin = 0;

    uint16_t timeBetweenTx = 0;
    uint16_t locationDeltaTx = 0;
    uint8_t speedDeltaTx = 0;

    String packetBuffer;
    String comment;

    long readVccAtmega();
    float convertDegMin(float decDeg);

    void stringPadding(int number, byte width, String *dest);

    void stringPaddingf(float number, byte width, String *dest);
    void buildPacket();
};


#endif //ARDUINO_TRACKER_APRS_DRA818_APRS_H
