#include <Arduino.h>
#include <ArduinoQAPRS.h>

#include "Utils.h"

#define TX_TIME_BETWEEN 30000
#define TX_SPEED_DIFFERENCE 30
#define TX_FREQ 144.800
#define TX_ENABLE

#define TIME_SERIAL_GPS 1000
#define GPS_DRA_BAUD 9600
#define APRS_COMMENT " TEST"

#define DRA_PTT 2
#define RX_GPS 6
#define RX_DRA 3
#define TX_DRA 4
#define DRA_ACTIVE 5
#define ALWAYS_TX_PIN 7

unsigned long lastTx = 0;
unsigned int lastSpeed = 0;

const char CALL[] = "F4HVV";
const char CALL_ID = '9';
const char TO_CALL[] = "CQ";
const char TO_CALL_ID = '0';
const char RELAYS[] = "WIDE1-1,WIDE2-1";


void setup() {
#ifdef DEBUG
  Serial.begin(GPS_DRA_BAUD);
#endif
  DPRINTLN(F("Start ..."));

  DPRINTLN(F("Init OK"));

  while (true || digitalRead(ALWAYS_TX_PIN)) {
    DPRINTLN(F("RUN TEST LOOP"));

#ifdef TX_ENABLE

#endif

    bool qaprsOk = QAPRS.sendData("F4HVV / APRS Arduino / TEST PACKET TEXTE / BEACON 10 sec") == QAPRSReturnOK;

    DPRINT(F("Packet OK: "));
    DPRINTLN(qaprsOk);



	  if (qaprsOk) {
        blink(2);
	  } else {
        blink(10);
	  }
	  delay(10000);
  }
}

void loop() {

  delay(1000);
}
