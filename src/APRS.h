#ifndef ARDUINO_TRACKER_APRS_DRA818_APRS_H
#define ARDUINO_TRACKER_APRS_DRA818_APRS_H

#include <Arduino.h>
#include "DRA.h"
#include "GPS.h"

class APRS {
public:
    APRS(DRA *dra, GPS *gps, unsigned int secondBetweenTx, double speedDeltaTx, uint8_t txPin);

    void init(char *call, uint8_t callId, char *toCall, uint8_t toCallId, char *relays);

    bool loop(bool test = false);

    void setSecondBetweenTx(unsigned int secondBetweenTx);

    void setSpeedDeltaTx(double speedDeltaTx);

    void setComment(String comment);

    bool txToRadio(String packet);

    bool sendPosition();

private:
    DRA *dra = nullptr;
    GPS *gps = nullptr;

    unsigned long lastTx = 0;
    double lastSpeed = 0;
    uint8_t txPin = 0;

    unsigned int timeBetweenTx = 0;
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
