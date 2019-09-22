#include <Arduino.h>

#include "Utils.h"
#include "GPS.h"
#include "DRA.h"
#include "APRS.h"

#define TX_SPEED_DIFFERENCE 30.0 // km/h
#define TX_LOCATION_DIFFERENCE 250 // m
#define TX_TIME_BETWEEN_NO_DRA 300 // 5 minutes

#ifdef TEST
#define TX_TIME_BETWEEN 10
#define TX_FREQ 144.600
#define APRS_COMMENT " MODE TEST 144.600 Mhz"
#else
#define TX_FREQ 144.800
#define TX_TIME_BETWEEN 90 // 1 minutes 30
#define APRS_COMMENT " https://frama.link/arduino-aprs"
#endif

#define DRA_PTT 2
#define TX_GPS 6 // from GPS to Arduino
#define TX_DRA 3 // from DRA to Arduino
#define RX_DRA 4 // from Arduino to DRA
#define DRA_ACTIVE 5
//#define BUTTON 12
#define PTT_OUT 12

char CALL[] = "F4HVV";
uint8_t callSsid = '9'; // car
char TO_CALL[] = "APRS";
uint8_t TO_CALL_ID = '0';
char RELAYS[] = "WIDE1-1,WIDE2-2";

bool isTestMode = IS_TEST_MODE;

GPS gps(TX_GPS);
DRA dra(TX_DRA, RX_DRA, DRA_PTT, DRA_ACTIVE);
APRS aprs(&dra, &gps, TX_TIME_BETWEEN, TX_SPEED_DIFFERENCE, TX_LOCATION_DIFFERENCE, PTT_OUT);

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

#ifdef DEBUG
    Serial.begin(9600);
#endif
    DPRINTLN(F("Starting ..."));

    if (!dra.init(TX_FREQ)) {
        callSsid = '7'; // walk
    }

    aprs.init(CALL, callSsid, TO_CALL, TO_CALL_ID, RELAYS);

    if (!dra.isDraDetected()) {
        aprs.setTimeBetweenTx(TX_TIME_BETWEEN_NO_DRA);
    }

    aprs.setComment(APRS_COMMENT);

    DPRINTLN(F("Started !"));
}

void loop() {
    aprs.loop(isTestMode);
#ifdef TEST
    if (millis() > 900000) { // 15 minutes
        aprs.setTimeBetweenTx(900); // 15 minutes
    }
#endif
    delay(1000);
}
