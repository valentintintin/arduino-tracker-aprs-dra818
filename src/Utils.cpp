#include <Arduino.h>

void blink(byte nb) {
    for (byte i = 0; i < nb; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(250);
        digitalWrite(LED_BUILTIN, LOW);
        delay(250);
    }
}
