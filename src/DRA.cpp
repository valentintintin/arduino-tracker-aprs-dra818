#include "DRA.h"
#include "Utils.h"

DRA::DRA(byte rxDra, byte txDra, byte pttPin, byte activePin) :
        pttPin(pttPin), activePin(activePin),
        dra(nullptr), activeState(false), txState(false) {
    serial = new SoftwareSerial(rxDra, txDra);
    serial->begin(9600);

    pinMode(pttPin, OUTPUT);
    pinMode(activePin, OUTPUT);
}

DRA::~DRA() {
    delete dra;
}

bool DRA::init(float freq, bool deactiveAfter, char loop) {
    digitalWrite(pttPin, HIGH);
    active();

    char i = loop;
    do {
        i--;
        DPRINTLN(F("DRA Init ..."));
        if (!(dra = DRA818::configure(serial, DRA818_VHF, freq, freq, 0, 0, 0, 0, DRA818_12K5, false, false, false,
                                      &Serial))) {
            DPRINTLN(F("DRA failed"));
            blink(15);
        }
    } while (dra == nullptr && i > 0);

    if (isDraDetected()) {
        digitalWrite(pttPin, LOW);
        delay(TIME_TOGGLE_PTT);
        digitalWrite(pttPin, HIGH);
        /*if (deactiveAfter) {
            deactive();
        }*/

        DPRINTLN(F("DRA OK"));

        return true;
    }

    DPRINTLN(F("No DRA"));

    return false;
}

void DRA::tx() {
    if (isDraDetected()) {
        /*if (!activeState) {
            active();
        }*/

        digitalWrite(pttPin, LOW);
        delay(TIME_TOGGLE_PTT);

        txState = true;
        DPRINTLN(F("DRA TX"));
    }
}

void DRA::stopTx(bool deactiveAfter) {
    if (isDraDetected()) {
        delay(100);

        digitalWrite(pttPin, HIGH);
        delay(TIME_TOGGLE_PTT);

        txState = false;

        /*if (deactiveAfter) {
            deactive();
        }*/
    }
}

bool DRA::isActive() {
    return activeState;
}

bool DRA::isTx() {
    return txState;
}

void DRA::active() {
    digitalWrite(activePin, HIGH);
    delay(TIME_TOGGLE_ACTIVE);

    activeState = true;
}

void DRA::deactive() {
    digitalWrite(activePin, LOW);

    activeState = false;
}

bool DRA::isDraDetected() {
    return dra != nullptr;
}
