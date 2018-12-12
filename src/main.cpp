#include <Arduino.h>

#include "Utils.h"
#include "GPS.h"
#include "DRA.h"
#include "APRS.h"

#define TX_SPEED_DIFFERENCE 30

#ifdef TEST
#define TX_TIME_BETWEEN 10
#define TX_FREQ 144.600
#define APRS_COMMENT " TEST APRS Arduino"
#else
#define TX_FREQ 144.800
#define TX_TIME_BETWEEN 900
#define APRS_COMMENT " 73 de Valentin"
#endif

#define DRA_PTT 2
#define RX_GPS 6
#define RX_DRA 3
#define TX_DRA 4
#define DRA_ACTIVE 5

char CALL[] = "F4HVV";
uint8_t CALL_ID = '9';
char TO_CALL[] = "CQ";
uint8_t TO_CALL_ID = '0';
char RELAYS[] = "WIDE1-1,WIDE2-1";

GPS gps(RX_GPS);
DRA dra(RX_DRA, TX_DRA, DRA_PTT, DRA_ACTIVE);
APRS aprs(&dra, &gps, TX_TIME_BETWEEN, TX_SPEED_DIFFERENCE);

void setup() {
#ifdef DEBUG
    Serial.begin(9600);
#endif
    DPRINTLN(F("Starting ..."));

    dra.init(TX_FREQ);

    aprs.init(CALL, CALL_ID, TO_CALL, TO_CALL_ID, RELAYS);

    aprs.setComment(APRS_COMMENT);

    DPRINTLN(F("Started !"));
}

void loop() {
    aprs.loop(IS_TEST_MODE);
#ifdef TEST
    if (millis() > 900000) { // 15 minutes
        aprs.setSecondBetweenTx(1800); // 30 minutes
    }
#endif
    delay(1000);
}
