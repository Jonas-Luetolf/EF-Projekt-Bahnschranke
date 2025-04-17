#include <Arduino.h>

#define SLAVE_ADDR 0x04
#define SDA_PIN 0
#define SCL_PIN 1


void setup() {
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);

}

void loop() {
    digitalWrite(4, HIGH);
    delay(100);
    digitalWrite(4, LOW);
    delay(100);
}