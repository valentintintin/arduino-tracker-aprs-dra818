#include <Arduino.h>

#include "Utils.h"
#include "GPS.h"
#include "DRA.h"
#include "APRS.h"

#define TX_TIME_BETWEEN 30
#define TX_SPEED_DIFFERENCE 30
#define TX_FREQ 144.800

#define APRS_COMMENT " TEST"

#define DRA_PTT 2
#define RX_GPS 6
#define RX_DRA 3
#define TX_DRA 4
#define DRA_ACTIVE 5
#define ALWAYS_TX_PIN 7

char BEACON[] = "F4HVV / APRS Arduino / TEST PACKET TEXTE / BEACON 10 sec";

const char CALL[] = "F4HVV";
const char CALL_ID = '9';
const char TO_CALL[] = "CQ";
const char TO_CALL_ID = '0';
const char RELAYS[] = "WIDE1-1,WIDE2-1";

GPS gps(RX_GPS);
DRA dra(RX_DRA, TX_DRA, DRA_PTT, DRA_ACTIVE);
APRS aprs(&dra, &gps, CALL, CALL_ID, TO_CALL, TO_CALL_ID, RELAYS, TX_TIME_BETWEEN, TX_SPEED_DIFFERENCE);

void setup() {
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
#else
    aprs.loop();
#endif
    delay(1000);
}
