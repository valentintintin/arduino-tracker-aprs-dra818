#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <ArduinoQAPRS.h>

#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
#ifdef DEBUG    //Macros are usually in all capital letters.
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
#else
  #define DPRINT(...)     //now defines a blank line
  #define DPRINTLN(...)   //now defines a blank line
#endif

#define TX_TIME_BETWEEN 2500

#define RX_GPS 3
#define TX_GPS 4
#define GPS_BAUD 9600
#define LED_PIN 13

TinyGPSPlus gps;
SoftwareSerial serialGPS(RX_GPS, TX_GPS);
unsigned long lastTx = 0;

char packetBuffer[255] = {'\0'};
char floatString[16];
const char CALL[] = "F4HVV";
const char CALL_SSID = '9';
const char TO_CALL[] = "CQ";
const char TO_CALL_SSID = '0';
const char RELAYS[] = "WIDE1-1,WIDE2-1";

void setup() {
  QAPRS.init(2, LED_PIN, CALL, CALL_SSID, TO_CALL, TO_CALL_SSID, RELAYS);

  #ifdef DEBUG
    Serial.begin(115200);
    DPRINTLN(F("Init OK"));
  #endif
}

void loop()  {
  if (millis() - lastTx >= TX_TIME_BETWEEN) {
    if (getGPSData(1000)) {
      
      txToRadio();
    }

    lastTx = millis();
  }

  delay(10);
}

long readVccAtmega() {  
  long result; // Read 1.1V reference against AVcc 
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); 
  delay(2); // Wait for Vref to settle 
  ADCSRA |= _BV(ADSC); // Convert 
  while (bit_is_set(ADCSRA,ADSC)); 
  result = ADCL; 
  result |= ADCH<<8; 
  result = 1126400L / result; // Back-calculate AVcc in mV 
  return result; 
}

bool getGPSData(unsigned long ms)
{
  DPRINTLN(F("GPS ..."));
  serialGPS.begin(GPS_BAUD);
  unsigned long start = millis();
  do 
  {
    while (serialGPS.available())
      gps.encode(serialGPS.read());
  } while (millis() - start < ms);
  serialGPS.flush();
  serialGPS.end();

  if (gps.charsProcessed() < 10) {
    DPRINTLN(F("GPS FAILED"));
    return false;
  } else if (!gps.location.isValid()) {
    DPRINTLN(F("GPS NO FIX"));
    return false;
  }

  #ifdef DEBUG
    displayInfo();
  #endif
  
  DPRINTLN(F("GPS OK"));
  return true;
}

float convertDegMin(float decDeg) {      
  float DegMin;
 
  int intDeg = decDeg; // partie entière
  decDeg -= intDeg; // partie décimale
  decDeg *= 60; // décimale * 60
  DegMin = ( intDeg*100 ) + decDeg; // entière * 100 + décimale
 
  return DegMin; 
}

void padding(int number, byte width, char *dest) {
  int temp = number;
  if (!temp) { temp++; }
    
  for (int i = 0; i < width - (log10(temp)) - 1; i++) {
    strcat(dest, "0");
  }
  sprintf(dest, "%s%d", dest, number);
}

void paddingf(float number, byte width, char *dest, char *tmpStr) {
  float temp = number;
  if (!temp) { temp++; }

  tmpStr[0] = '\0';
  
  for (int i = 0; i < width - (log10(temp)) - 1; i++) {
    strcat(dest, "0");
  }
  dtostrf(number, strlen(tmpStr), 2, tmpStr);
  sprintf(dest, "%s%s", dest, tmpStr);
}

void txToRadio() {
  DPRINTLN(F("TX ..."));
  float lat = gps.location.lat(), lng = gps.location.lng(), latDegMin = convertDegMin(lat), lngDegMin = convertDegMin(lng);

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
  padding((int) gps.course.deg(), 3, packetBuffer);
  // Separator
  strcat(packetBuffer, "/");
  // Speed
  padding((int) gps.speed.knots(), 3, packetBuffer);  
  // Altitude
  strcat(packetBuffer, " /A=");
  padding((int) gps.altitude.feet(), 6, packetBuffer);
  // Voltage
  sprintf(packetBuffer, "%s V=%d", packetBuffer, readVccAtmega());
  // Satelite
  strcat(packetBuffer, " Sat=");
  padding((int) gps.satellites.value(), 6, packetBuffer);
  // Comment
  strcat(packetBuffer, " TEST");

  DPRINTLN(packetBuffer);

  QAPRS.sendData(packetBuffer);
  
  DPRINTLN(F("TX OK"));
}


void displayInfo() {
  DPRINT(F("Location: ")); 
  if (gps.location.isValid())
  {
    DPRINT(gps.location.lat(), 6);
    DPRINT(F(","));
    DPRINT(gps.location.lng(), 6);
  }
  else
  {
    DPRINT(F("INVALID"));
  }

  DPRINT(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    DPRINT(gps.date.month());
    DPRINT(F("/"));
    DPRINT(gps.date.day());
    DPRINT(F("/"));
    DPRINT(gps.date.year());
  }
  else
  {
    DPRINT(F("INVALID"));
  }

  DPRINT(F(" "));
  if (gps.time.isValid())
  {
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
  }
  else
  {
    DPRINT(F("INVALID"));
  }

  DPRINTLN();
}
