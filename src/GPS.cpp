#include "GPS.h"
#include "Utils.h"

GPS::GPS(byte rxPin, int baud, byte txPin) : baud(baud) {
    serial = new SoftwareSerial(rxPin, txPin);
}

bool GPS::getData() {
    DPRINTLN(F("GPS update..."));

    serial->begin(baud);
    uint32_t start = millis();
    do {
        while (serial->available()) {
            char r = (char) serial->read();
//            DPRINT(r);
            gps.encode(r);
        }
    } while (millis() - start < TIME_WAIT_DATA);
    serial->flush();
    serial->end();

    if (!gps.location.isValid()) {
        DPRINT(F("GPS no fix, sat: ")); DPRINTLN(gps.satellites.value());
        return false;
    }
    DPRINTLN(F("GPS OK"));
#ifdef DEBUG
    displayInfo();
#endif
    return true;
}


void GPS::displayInfo() {
    DPRINT(F("Sat: ")); DPRINT(gps.satellites.value());
    DPRINT(F(" HDOP: ")); DPRINTLN(gps.hdop.hdop(), 0);

    DPRINT(F("Lat: ")); DPRINTLN(gps.location.lat(), 6);
    DPRINT(F("Lng: ")); DPRINTLN(gps.location.lng(), 6);

    DPRINT(F("Alt: ")); DPRINT(gps.altitude.meters(), 0);
    DPRINT(F(" Spd: ")); DPRINT(gps.speed.kmph(), 0);
    DPRINT(F(" Crs: ")); DPRINTLN(gps.course.deg(), 0);

#ifdef DEBUG
    delay(2000);
#endif
}
