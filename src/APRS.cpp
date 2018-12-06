#include <ArduinoQAPRS.h>
#include "APRS.h"
#include "Utils.h"

APRS::APRS(DRA *dra, GPS *gps,
           const char *call, char callId, const char *toCall, char toCallId, const char *relays,
           int secondBetweenTx, byte speedDeltaTx) :
        dra(dra), gps(gps), timeBetweenTx(secondBetweenTx * 1000), speedDeltaTx(speedDeltaTx),
        call(call), callId(callId), toCall(toCall), toCallId(toCallId), relays(relays) {
}

bool APRS::txToRadio(char *packet) {
    DPRINTLN(F("TX ..."));

    dra->tx();

    bool qaprsOk = QAPRS.sendData(packet) == QAPRSReturnOK;

    dra->stopTx();

    if (qaprsOk) {
        DPRINTLN(F("APRS OK"));
    } else {
        DPRINTLN(F("APRS FAILED"));
    }

    return qaprsOk;
}

bool APRS::loop() {
    if (gps->getData()) {
        if (millis() - lastTx >= timeBetweenTx ||
            lastSpeed - gps->gps->speed.kmph() >= speedDeltaTx) {
            if (sendPosition()) {
                lastTx = millis();
                blink(2);
            }
        } else {
            DPRINT(F("Next:"));
            DPRINTLN(timeBetweenTx - (millis() - lastTx));
        }
    } else {
        blink(5);
    }
}

long APRS::readVccAtmega() {
    long result;  // Read 1.1V reference against AVcc
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    delay(2);             // Wait for Vref to settle
    ADCSRA |= _BV(ADSC);  // Convert
    while (bit_is_set(ADCSRA, ADSC));
    result = ADCL;
    result |= ADCH << 8;
    result = 1126400L / result;  // Back-calculate AVcc in mV
    return result;
}

float APRS::convertDegMin(float decDeg) {
    float DegMin;

    int intDeg = decDeg;               // partie entière
    decDeg -= intDeg;                  // partie décimale
    decDeg *= 60;                      // décimale * 60
    DegMin = (intDeg * 100) + decDeg;  // entière * 100 + décimale

    return DegMin;
}

void APRS::stringPadding(int number, byte width, char *dest) {
    int temp = number;
    if (!temp) {
        temp++;
    }

    for (int i = 0; i < width - (log10(temp)) - 1; i++) {
        strcat(dest, "0");
    }
    sprintf(dest, "%s%d", dest, number);
}

void APRS::stringPaddingf(float number, byte width, char *dest, char *tmpStr) {
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

void APRS::buildPacket() {
    float lat = gps->gps->location.lat();
    float lng = gps->gps->location.lng();
    float latDegMin = convertDegMin(lat);
    float lngDegMin = convertDegMin(lng);

    packetBuffer[0] = '\0';

    // Start coordinates
    strcat(packetBuffer, "!");

    // Latitude
    stringPaddingf(latDegMin, 4, packetBuffer, floatString);
    // Determine N or S
    if (latDegMin >= 0) {
        strcat(packetBuffer, "N");
    } else if (latDegMin < 0) {
        strcat(packetBuffer, "S");
    }

    // Separator
    strcat(packetBuffer, "/");

    // Longitude
    stringPaddingf(lngDegMin, 5, packetBuffer, floatString);
    // Determine E or W
    if (lngDegMin >= 0) {
        strcat(packetBuffer, "E");
    } else if (lngDegMin < 0) {
        strcat(packetBuffer, "W");
    }

    // Symbol car
    strcat(packetBuffer, ">");

    // North orientation
    stringPadding((int) gps->gps->course.deg(), 3, packetBuffer);
    // Separator
    strcat(packetBuffer, "/");
    // Speed
    stringPadding((int) gps->gps->speed.knots(), 3, packetBuffer);
    // Altitude
    strcat(packetBuffer, " /A=");
    stringPadding((int) gps->gps->altitude.feet(), 6, packetBuffer);
    // Voltage
    sprintf(packetBuffer, "%s V=%ld", packetBuffer, readVccAtmega());
    // Satelite
    strcat(packetBuffer, " Sat=");
    stringPadding((int) gps->gps->satellites.value(), 6, packetBuffer);
// Comment
#ifdef APRS_COMMENT
    strcat(packetBuffer, APRS_COMMENT);
#endif

    DPRINTLN(packetBuffer);
}

bool APRS::sendPosition() {
    buildPacket();
    return txToRadio(packetBuffer);
}
