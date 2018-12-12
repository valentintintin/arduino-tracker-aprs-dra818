#ifndef ARDUINO_TRACKER_APRS_DRA818_APRS_H
#define ARDUINO_TRACKER_APRS_DRA818_APRS_H

#include <Arduino.h>
#include "DRA.h"
#include "GPS.h"

class APRS {
public:
    APRS(DRA *dra, GPS *gps, const char *call, char callId, const char *toCall = "CQ", char toCallId = '0',
         const char *relays = "WIDE1-1,WIDE2-1", int secondBetweenTx = 30, byte speedDeltaTx = 30);

    bool loop();

    void setSecondBetweenTx(unsigned int secondBetweenTx);

    void setSpeedDeltaTx(byte speedDeltaTx);
    void setComment(const char *comment);
    bool txToRadio(char *packet);

private:
    DRA *dra = nullptr;
    GPS *gps = nullptr;

    unsigned long lastTx = 0;
    byte lastSpeed = 0;

    unsigned int timeBetweenTx;
    byte speedDeltaTx;

    char packetBuffer[255] = {'\0'};
    char floatString[16] = {'\0'};

    const char *call;
    char callId;
    const char *toCall;
    char toCallId;
    const char *relays;

    const char *comment = nullptr;

    long readVccAtmega();

    float convertDegMin(float decDeg);

    void stringPadding(int number, byte width, char *dest);

    void stringPaddingf(float number, byte width, char *dest, char *tmpStr);

    void buildPacket();

    bool sendPosition();
};


#endif //ARDUINO_TRACKER_APRS_DRA818_APRS_H
