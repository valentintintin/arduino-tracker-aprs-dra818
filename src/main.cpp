#include <Arduino.h>
#include "Utils.h"
#include "GPS.h"
#include "DRA.h"
#include "APRS.h"

#ifdef USE_BUTTON
    #include <EasyButton.h>
#endif

#ifdef CONTINUE_TX
    #include <ArduinoQAPRS.h>
#endif

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
#define BUTTON 7

char CALL[] = "F4HVV";
uint8_t callSsid = '9'; // car
char TO_CALL[] = "APFD38"; // AP = AP Packet + F = France + D38 = department 38 in France
uint8_t TO_CALL_ID = '0';
char RELAYS[] = "WIDE1-1,WIDE2-2";

bool isTestMode = IS_TEST_MODE;

GPS gps(TX_GPS);
DRA dra(TX_DRA, RX_DRA, DRA_PTT, DRA_ACTIVE);
APRS aprs(&dra, &gps, PTT_OUT);

#ifdef USE_BUTTON
    EasyButton button(BUTTON);
#endif

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);

    Serial.begin(115200);

    DPRINTLN(F("Starting ..."));

#ifdef USE_BUTTON
    button.begin();
    button.read();

    if (button.isPressed()) {
        isTestMode = !isTestMode;
        blink(3 + isTestMode);
    }
#endif

#ifdef TX_30_S
    DPRINTLN(F("TX every 30 seconds when GPS fixed"));
#endif

    if (!dra.init(TX_FREQ)) {
        callSsid = '7'; // walk
    }

    aprs.init(CALL, callSsid, TO_CALL, TO_CALL_ID, RELAYS);

    aprs.setComment(APRS_COMMENT);

    DPRINT(F("Test mode: ")); DPRINTLN(isTestMode);
    DPRINT(F("Call: ")); DPRINT(CALL); DPRINT(F("-")); DPRINTLN(callSsid);
    DPRINT(F("Relay: ")); DPRINTLN(RELAYS);
    DPRINT(F("Frequency: ")); DPRINTLN(TX_FREQ);
    DPRINT(F("Comment: ")); DPRINTLN(APRS_COMMENT);

    DPRINTLN(F("Started !"));

#ifdef CONTINUE_TX
    dra.tx();
    digitalWrite(PTT_OUT, HIGH);
    char* bufferAx25 = "F4HVV test de F4HVV test";
    while (true) {
        QAPRS.sendData(bufferAx25);
    }
#endif

#ifdef USE_BUTTON
    button.onPressedFor(5000, []() {
        DPRINTLN(F("TX Forced"));
        aprs.sendIfPossible(isTestMode, true);
    });
#endif
}

void loop() {
#ifdef USE_BUTTON
    button.read();
#endif
    aprs.sendIfPossible(isTestMode, isTestMode);
}
