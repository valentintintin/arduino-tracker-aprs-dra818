#ifndef ARDUINO_TRACKER_APRS_DRA818_DRA_H
#define ARDUINO_TRACKER_APRS_DRA818_DRA_H

#include <Arduino.h>
#include <DRA818.h>
#include <SoftwareSerial.h>

#define TIME_TOGGLE_ACTIVE 1500
#define TIME_TOGGLE_PTT 500

class DRA {
public:
    DRA(byte rxDra, byte txDra, byte pttPin, byte activePin);

    virtual ~DRA();

    bool init(float txFreq = 144.800, bool deactiveAfter = true, char loop = 1);

    void tx();

    void stopTx(bool deactiveAfter = true);

    bool isTx();
    bool isActive();

    bool isDraDetected();

    DRA818 *dra;

private:
    void active();

    void deactive();

    SoftwareSerial *serial;

    byte pttPin;
    byte activePin;

    bool activeState;
    bool txState;
};


#endif //ARDUINO_TRACKER_APRS_DRA818_DRA_H
