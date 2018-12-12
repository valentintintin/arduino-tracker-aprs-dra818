#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <ArduinoQAPRS.h>
#include <DRA818.h>

#define DEBUG
#ifdef DEBUG
#define DPRINT(...) Serial.print(__VA_ARGS__)
#define DPRINTLN(...) \
  Serial.println(__VA_ARGS__)
#else
#define DPRINT(...)
#define DPRINTLN(...)
#endif

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
#define LED_PIN 13
#define ALWAYS_TX_PIN 7

TinyGPSPlus gps;
SoftwareSerial serialGPS(RX_GPS, 0);
SoftwareSerial serialDRA(RX_DRA, TX_DRA);
DRA818 *dra;

unsigned long lastTx = 0;
unsigned int lastSpeed = 0;

char packetBuffer[255] = {'\0'};
char floatString[16];
char CALL[] = "F4HVV";
char CALL_ID = '9';
char TO_CALL[] = "CQ";
char TO_CALL_ID = '0';
char RELAYS[] = "WIDE1-1,WIDE2-1";

void blink(byte nb) {
	for (byte i = 0; i < nb + 1; i++) {
		digitalWrite(13, HIGH);
		delay(350);
		digitalWrite(13, LOW);
		delay(350);
	}
}

void displayInfo() {
  DPRINT(F("Location: "));
  if (gps.location.isValid()) {
    DPRINT(gps.location.lat(), 6);
    DPRINT(F(","));
    DPRINT(gps.location.lng(), 6);
    DPRINT(F(" Alt: "));
    DPRINT(gps.altitude.meters(), 2);
    DPRINT(F(" V: "));
    DPRINT(gps.speed.kmph(), 2);
  } else {
    DPRINT(F("INVALID"));
  }

  DPRINT(F("  Date/Time: "));
  if (gps.date.isValid()) {
    DPRINT(gps.date.month());
    DPRINT(F("/"));
    DPRINT(gps.date.day());
    DPRINT(F("/"));
    DPRINT(gps.date.year());
  } else {
    DPRINT(F("INVALID"));
  }

  DPRINT(F(" "));
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10) DPRINT(F("0"));
    DPRINT(gps.time.hour());
    DPRINT(F(":"));
    if (gps.time.minute() < 10) DPRINT(F("0"));
    DPRINT(gps.time.minute());
    DPRINT(F(":"));
    if (gps.time.second() < 10) DPRINT(F("0"));
    DPRINT(gps.time.second());
    DPRINT(F("."));
    if (gps.time.centisecond() < 10) DPRINT(F("0"));
    DPRINT(gps.time.centisecond());
  } else {
    DPRINT(F("INVALID"));
  }

  DPRINTLN();
}

long readVccAtmega() {
  long result;  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2);             // Wait for Vref to settle
  ADCSRA |= _BV(ADSC);  // Convert
  while (bit_is_set(ADCSRA, ADSC))
    ;
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result;  // Back-calculate AVcc in mV
  return result;
}

bool getGPSData(unsigned long ms) {
  DPRINTLN(F("GPS INIT..."));
  serialGPS.begin(GPS_DRA_BAUD);

  unsigned long start = millis();
  do {
    while (serialGPS.available()) gps.encode(serialGPS.read());
  } while (millis() - start < ms);
  serialGPS.flush();
  serialGPS.end();

  if (gps.charsProcessed() < 10) {
    DPRINTLN(F("GPS FAILED"));
    return false;
  } else if (!gps.location.isValid()) {
    DPRINT(F("GPS NO FIX, SATELLITES: "));
    DPRINTLN(gps.satellites.value());
    return false;
  }

  DPRINTLN(F("GPS OK"));

#ifdef DEBUG
  displayInfo();
#endif

  return true;
}

float convertDegMin(float decDeg) {
  float DegMin;

  int intDeg = decDeg;               // partie entière
  decDeg -= intDeg;                  // partie décimale
  decDeg *= 60;                      // décimale * 60
  DegMin = (intDeg * 100) + decDeg;  // entière * 100 + décimale

  return DegMin;
}

void padding(int number, byte width, char* dest) {
  int temp = number;
  if (!temp) {
    temp++;
  }

  for (int i = 0; i < width - (log10(temp)) - 1; i++) {
    strcat(dest, "0");
  }
  sprintf(dest, "%s%d", dest, number);
}

void paddingf(float number, byte width, char* dest, char* tmpStr) {
  float temp = number;
  if (!temp) {
    temp++;
  }

  tmpStr[0] = '\0';

  for (int i = 0; i < width - (log10(temp)) - 1; i++) {
    strcat(dest, "0");
  }
  dtostrf(number, strlen(tmpStr), 2, tmpStr);
  sprintf(dest, "%s%s", dest, tmpStr);
}

bool txToRadio() {
  DPRINTLN(F("TX ..."));
  float lat = gps.location.lat(), lng = gps.location.lng(),
          latDegMin = convertDegMin(lat), lngDegMin = convertDegMin(lng);

  packetBuffer[0] = '\0';

  // Start coordinates
  strcat(packetBuffer, "!");

  // Latitude
  paddingf(latDegMin, 4, packetBuffer, floatString);
  // Determine N or S
  if (latDegMin >= 0) {
    strcat(packetBuffer, "N");
  } else if (latDegMin < 0) {
    strcat(packetBuffer, "S");
  }

  // Separator
  strcat(packetBuffer, "/");

  // Longitude
  paddingf(lngDegMin, 5, packetBuffer, floatString);
  // Determine E or W
  if (lngDegMin >= 0) {
    strcat(packetBuffer, "E");
  } else if (lngDegMin < 0) {
    strcat(packetBuffer, "W");
  }

  // Symbol car
  strcat(packetBuffer, ">");

  // North orientation
  padding((int)gps.course.deg(), 3, packetBuffer);
  // Separator
  strcat(packetBuffer, "/");
  // Speed
  padding((int)gps.speed.knots(), 3, packetBuffer);
  // Altitude
  strcat(packetBuffer, " /A=");
  padding((int)gps.altitude.feet(), 6, packetBuffer);
  // Voltage
  sprintf(packetBuffer, "%s V=%ld", packetBuffer, readVccAtmega());
  // Satelite
  strcat(packetBuffer, " Sat=");
  padding((int)gps.satellites.value(), 6, packetBuffer);
// Comment
#ifdef APRS_COMMENT
  strcat(packetBuffer, APRS_COMMENT);
#endif

  DPRINTLN(packetBuffer);

#ifdef TX_ENABLE
  DPRINTLN(F("DRA TX"));
  digitalWrite(DRA_ACTIVE, HIGH);
  delay(1500);
  digitalWrite(DRA_PTT, LOW);
  delay(500);
#endif

  bool qaprsOk = QAPRS.sendData(packetBuffer) == QAPRSReturnOK;

  DPRINT(F("APRS OK: ")); DPRINTLN(qaprsOk);

  delay(100);
  digitalWrite(DRA_PTT, HIGH);
  digitalWrite(DRA_ACTIVE, LOW);
  delay(500);

  return qaprsOk;
}

void setup() {
#ifdef DEBUG
  Serial.begin(GPS_DRA_BAUD);
#endif
  DPRINTLN(F("Start ..."));

  pinMode(DRA_PTT, OUTPUT);
  pinMode(DRA_ACTIVE, OUTPUT);

  digitalWrite(DRA_PTT, HIGH);
  digitalWrite(DRA_ACTIVE, HIGH);

  serialDRA.begin(GPS_DRA_BAUD);

  do {
    DPRINTLN(F("DRA INIT ..."));
    if (!(dra = DRA818::configure(&serialDRA, DRA818_VHF, TX_FREQ, TX_FREQ, 0, 0, 0,
                                  0, DRA818_12K5, false, false, false, &Serial))) {
      DPRINTLN(F("DRA ERROR"));
      blink(10);
    }
  } while(!dra);

  digitalWrite(DRA_PTT, LOW);
  delay(300);
  digitalWrite(DRA_PTT, HIGH);
  DPRINTLN(F("DRA OK"));
  digitalWrite(DRA_ACTIVE, LOW);

  QAPRS.init(2, LED_PIN, CALL, CALL_ID, TO_CALL, TO_CALL_ID, RELAYS);
  DPRINTLN(F("Init OK"));
  /*
  while (digitalRead(ALWAYS_TX_PIN)) {
	  bool qaprsOk = QAPRS.sendData("F4HVV / APRS Arduino / TEST PACKET TEXTE / BEACON 10 sec") == QAPRSReturnOK;
	  if (qaprsOk) {
	  	blink(2);
	  } else {
	  	blink(10);
	  }
	  delay(10000);
  }*/
}

void loop() {
  if (getGPSData(TIME_SERIAL_GPS)) {
    if (millis() - lastTx >= TX_TIME_BETWEEN ||
        lastSpeed - gps.speed.kmph() >= TX_SPEED_DIFFERENCE) {
      if (txToRadio()) {
        lastTx = millis();
        blink(2);
      }
    } else {
      DPRINT(F("Next:"));
      DPRINTLN(TX_TIME_BETWEEN - (millis() - lastTx));
    }
  } else {
  	blink(5);
  }

  delay(1000);
}