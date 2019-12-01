#include <Arduino.h>

#include "Utils.h"
#include "GPS.h"
#include "DRA.h"
#include "APRS.h"

#ifdef TEST
#define TX_FREQ 144.600
#define APRS_COMMENT " MODE TEST https://frama.link/arduino-aprs"
#else
#define TX_FREQ 144.800
#define APRS_COMMENT " https://frama.link/arduino-aprs"
#endif

#define DRA_PTT 2
#define TX_GPS 6 // from GPS to Arduino
#define TX_DRA 3 // from DRA to Arduino
#define RX_DRA 4 // from Arduino to DRA
#define DRA_ACTIVE 5
#define PTT_OUT 12

char CALL[] = "F4HVV";
uint8_t callSsid = '9'; // car
char TO_CALL[] = "APFD38"; // AP = AP Packet + F = France + D38 = department 38 in France
uint8_t TO_CALL_ID = '0';
char RELAYS[] = "WIDE1-1,WIDE2-2";

bool isTestMode = IS_TEST_MODE;

GPS gps(TX_GPS);
DRA dra(TX_DRA, RX_DRA, DRA_PTT, DRA_ACTIVE);
APRS aprs(&dra, &gps, PTT_OUT);

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

#ifdef DEBUG
    Serial.begin(115200);
#endif
    DPRINTLN(F("Starting ..."));

    if (!dra.init(TX_FREQ)) {
        callSsid = '7'; // walk
    }

    aprs.init(CALL, callSsid, TO_CALL, TO_CALL_ID, RELAYS);

    aprs.setComment(APRS_COMMENT);

    DPRINTLN(F("Started !"));
}

void loop() {
    aprs.loop(isTestMode);
    delay(5000);
}
