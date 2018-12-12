#include "GPS.h"
#include "Utils.h"

GPS::GPS(byte rxPin, int baud, byte txPin) : baud(baud) {
    serial = new SoftwareSerial(rxPin, txPin);
}

GPS::~GPS() {
    delete gps;
}

bool GPS::getData() {
    DPRINTLN(F("GPS INIT..."));

    serial->begin(baud);
    unsigned long start = millis();
    do {
        while (serial->available()) gps->encode(serial->read());
    } while (millis() - start < TIME_WAIT_DATA);
    serial->flush();
    serial->end();

    DPRINT("GPS Char processed : ");
    DPRINTLN(gps->charsProcessed());

    if (gps->charsProcessed() < 10) {
        DPRINTLN(F("GPS FAILED"));
        return false;
    } else if (!gps->location.isValid()) {
        DPRINT(F("GPS NO FIX, SATELLITES: "));
        DPRINTLN(gps->satellites.value());
        return false;
    }
    DPRINTLN(F("GPS OK"));
#ifdef DEBUG
    displayInfo();
#endif
    return true;
}


void GPS::displayInfo() {
    DPRINT(F("Location: "));
    if (gps->location.isValid()) {
        DPRINT(gps->location.lat(), 6);
        DPRINT(F(","));
        DPRINT(gps->location.lng(), 6);
        DPRINT(F(" Alt: "));
        DPRINT(gps->altitude.meters(), 2);
        DPRINT(F(" V: "));
        DPRINT(gps->speed.kmph(), 2);
    } else {
        DPRINT(F("INVALID"));
    }

    DPRINT(F("  Date/Time: "));
    if (gps->date.isValid()) {
        DPRINT(gps->date.month());
        DPRINT(F("/"));
        DPRINT(gps->date.day());
        DPRINT(F("/"));
        DPRINT(gps->date.year());
    } else {
        DPRINT(F("INVALID"));
    }

    DPRINT(F(" "));
    if (gps->time.isValid()) {
        if (gps->time.hour() < 10) DPRINT(F("0"));
        DPRINT(gps->time.hour());
        DPRINT(F(":"));
        if (gps->time.minute() < 10) DPRINT(F("0"));
        DPRINT(gps->time.minute());
        DPRINT(F(":"));
        if (gps->time.second() < 10) DPRINT(F("0"));
        DPRINT(gps->time.second());
        DPRINT(F("."));
        if (gps->time.centisecond() < 10) DPRINT(F("0"));
        DPRINT(gps->time.centisecond());
    } else {
        DPRINT(F("INVALID"));
    }

    DPRINTLN();
}
