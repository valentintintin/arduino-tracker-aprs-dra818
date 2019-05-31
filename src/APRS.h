#ifndef ARDUINO_TRACKER_APRS_DRA818_APRS_H
#define ARDUINO_TRACKER_APRS_DRA818_APRS_H

#include <Arduino.h>
#include "DRA.h"
#include "GPS.h"

class APRS {
public:
    APRS(DRA *dra, GPS *gps, uint8_t secondBetweenTx, double speedDeltaTx, uint8_t locationMeterDeltaTx, uint8_t txPin);

    void init(char *call, uint8_t callId, char *toCall, uint8_t toCallId, char *relays);

    bool loop(bool test = false);

    void setTimeBetweenTx(uint8_t timeBetweenTx);

    void setSpeedDeltaTx(double speedDeltaTx);

    void setLocationDeltaTx(double localtionDeltaTx);

    void setComment(String comment);

    bool txToRadio(String packet);

    bool sendPosition();

private:
    DRA *dra = nullptr;
    GPS *gps = nullptr;

    uint32_t lastTx = 0;
    double lastSpeed = 0;
    double lastLat = 0;
    double lastLng = 0;
    uint32_t nbSent = 0;


    uint8_t txPin = 0;

    uint8_t timeBetweenTx = 0;
    uint8_t locationDeltaTx = 0;
    double speedDeltaTx = 0;

    String packetBuffer;
    String comment;

    long readVccAtmega();
    float convertDegMin(float decDeg);

    void stringPadding(int number, byte width, String *dest);

    void stringPaddingf(float number, byte width, String *dest);
    void buildPacket();
};


#endif //ARDUINO_TRACKER_APRS_DRA818_APRS_H
