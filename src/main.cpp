#include <Arduino.h>

#include "Utils.h"
#include "GPS.h"
#include "DRA.h"
#include "APRS.h"

#define TX_TIME_BETWEEN 5
#define TX_SPEED_DIFFERENCE 30

#ifndef TEST
#define TX_FREQ 144.800
#else
#define TX_FREQ 144.600
#endif

#define APRS_COMMENT " TEST APRS Arduino"

#define DRA_PTT 2
#define RX_GPS 6
#define RX_DRA 3
#define TX_DRA 4
#define DRA_ACTIVE 5

char CALL[] = "F4HVV";
char CALL_ID = '9';
char TO_CALL[] = "CQ";
char TO_CALL_ID = '0';
char RELAYS[] = "WIDE1-1,WIDE2-1";

char BEACON[] = APRS_COMMENT;

GPS gps(RX_GPS);
DRA dra(RX_DRA, TX_DRA, DRA_PTT, DRA_ACTIVE);
APRS aprs(&dra, &gps, CALL, CALL_ID, TO_CALL, TO_CALL_ID, RELAYS, TX_TIME_BETWEEN, TX_SPEED_DIFFERENCE);

void setup() {
#ifdef DEBUG
    Serial.begin(9600);
#endif
    DPRINTLN(F("Starting ..."));

    aprs.setComment(APRS_COMMENT);

    dra.init(TX_FREQ);

    DPRINTLN(F("Started !"));
}

void loop() {
#ifdef TEST
    DPRINTLN(F("Test ..."));
    if (aprs.txToRadio(BEACON)) {
        DPRINTLN(F("OK !"));
        blink(2);
    } else {
        DPRINTLN(F("FAILED !"));
        blink(10);
    }
    delay(TX_TIME_BETWEEN * 1000);
#else
    aprs.loop();
#endif
    delay(1000);
}
