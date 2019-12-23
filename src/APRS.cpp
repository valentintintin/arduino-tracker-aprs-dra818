#include <ArduinoQAPRS.h>
#include "APRS.h"
#include "Utils.h"

APRS::APRS(DRA *dra, GPS *gps, uint8_t txPin) : dra(dra), gps(gps), txPin(txPin) {
    pinMode(txPin, OUTPUT);
    packetBuffer.reserve(255);
}

void APRS::init(char *call, uint8_t callId, char *toCall, uint8_t toCallId, char *relays) {
    QAPRS.init(0, 0, call, callId, toCall, toCallId, relays);
}

bool APRS::txToRadio() {
    DPRINTLN(F("TX ..."));

    digitalWrite(LED_BUILTIN, HIGH);
    if (!dra->isDraDetected()) {
        digitalWrite(txPin, HIGH);
        delay(TIME_TOGGLE_PTT);
    } else {
        dra->tx();
    }

    delay(500);

    DPRINTLN(F("Packet sending"));

    bool qaprsOk = QAPRS.sendData((char *) packetBuffer.c_str()) == QAPRSReturnOK;

    DPRINTLN(F("Packet sent"));

    delay(500);

    digitalWrite(LED_BUILTIN, LOW);
    if (!dra->isDraDetected()) {
        digitalWrite(txPin, LOW);
        delay(TIME_TOGGLE_PTT);
    } else {
        dra->stopTx();
    }

    if (qaprsOk) {
        DPRINTLN(F("TX OK"));
    } else {
        DPRINTLN(F("TX FAILED"));
    }

    return qaprsOk;
}

void APRS::setComment(String comment) {
    this->comment = comment;
}

bool APRS::sendIfPossible(bool forceGps, bool forceTx) {
    if (gps->getData() || forceGps) {
        if (millis() - lastTx >= (uint32_t) 1000 * getTimeSecondsForGivenSpeed() || forceTx) {
            if (sendPosition()) {
                blink(2);
                lastTx = millis();
                lastSpeed = gps->gps.speed.kmph();
                return true;
            }
        }
        blink(1);

        DPRINT(F("Next: ")); DPRINT(abs(getTimeSecondsForGivenSpeed() - ((millis() - lastTx) / 1000)));
        DPRINT(F("/")); DPRINTLN(getTimeSecondsForGivenSpeed());

        #ifdef DEBUG
            delay(1000);
        #endif

        return false;
    } else {
        lastTx = 0;
        lastSpeed = 0;
        blink(5);
        return false;
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
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
#pragma clang diagnostic pop

uint16_t APRS::getTimeSecondsForGivenSpeed() {
    float speed = gps->gps.speed.kmph();
    if (speed >= 20) {
        return -0.1 * speed + 42;
    } else if (speed >= 5) {
        return -11 * speed + 230;
    } else if (lastSpeed >= 20 || lastTx == 0) {
        return 0;
    } else {
        return 300;
    }
}

float APRS::convertDegMin(float decDeg) {
    float DegMin;

    int intDeg = (int) decDeg;               // partie entière
    decDeg -= (float) intDeg;                  // partie décimale
    decDeg *= 60;                      // décimale * 60
    DegMin = (float) (intDeg * 100) + decDeg;  // entière * 100 + décimale

    return DegMin;
}

void APRS::stringPadding(int number, byte width, String *dest) {
    int temp = number;
    if (!temp) {
        temp++;
    }

    for (int i = 0; i < width - (log10(temp)) - 1; i++) {
        dest->concat('0');
    }
    dest->concat(number);
}

void APRS::stringPaddingf(float number, byte width, String *dest) {
    float temp = number;
    if (temp == 0) {
        temp++;
    }

    for (int i = 0; i < width - (log10(temp)) - 1; i++) { // NOLINT(performance-type-promotion-in-math-fn)
        dest->concat('0');
    }
    dest->concat(number);
}

void APRS::buildPacket() {
    auto lat = (float) gps->gps.location.lat();
    auto lng = (float) gps->gps.location.lng();
    float latDegMin = convertDegMin(lat);
    float lngDegMin = convertDegMin(lng);

    packetBuffer.remove(0, packetBuffer.length());

    // Start coordinates
    packetBuffer += '!';

    // Latitude
    stringPaddingf(latDegMin, 4, &packetBuffer);
    // Determine N or S
    if (latDegMin >= 0) {
        packetBuffer += 'N';
    } else if (latDegMin < 0) {
        packetBuffer += 'N';
    }

    // Separator
    packetBuffer += '/';

    // Longitude
    stringPaddingf(lngDegMin, 5, &packetBuffer);
    // Determine E or W
    if (lngDegMin >= 0) {
        packetBuffer += 'E';
    } else if (lngDegMin < 0) {
        packetBuffer += 'W';
    }

    if (dra->isDraDetected()) {
        // Symbol car
        packetBuffer += '>';
    } else {
        // Symbol human
        packetBuffer += '[';
    }

    // North orientation
    stringPadding((int) gps->gps.course.deg(), 3, &packetBuffer);
    // Separator
    packetBuffer += '/';
    // Speed
    stringPadding((int) gps->gps.speed.knots(), 3, &packetBuffer);
    // Altitude
    packetBuffer += "/A=";
    stringPadding((int) gps->gps.altitude.feet(), 6, &packetBuffer);
    // Voltage
    packetBuffer += "/V=";
    packetBuffer += (float) readVccAtmega() / 1000.f;
    // Accuracy
    packetBuffer += " HDOP=";
    packetBuffer += (int) gps->gps.hdop.hdop();
    // Comment
    if (comment.length()) {
        packetBuffer += comment;
    }

    DPRINT(F("Packet : "));
    DPRINTLN(packetBuffer);
}

bool APRS::sendPosition() {
    buildPacket();
    return txToRadio();
}
