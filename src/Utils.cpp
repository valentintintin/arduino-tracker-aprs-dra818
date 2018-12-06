#include <Arduino.h>

#define LED_PIN 13

void blink(byte nb) {
    for (byte i = 0; i < nb + 1; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(250);
        digitalWrite(LED_PIN, LOW);
        delay(250);
    }
}
