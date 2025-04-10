#include <Arduino.h>
#include <Servo.h>

Servo myServo;

#define SLAVE_ADDR 0x03

#define servoSpeed 15 // 15 als Defaultwert ist gut für Schranke
#define startAngle 0
#define targetAngle 180

void setup() {
    Serial.begin(115200);
    myServo.attach(9); // GP9 -> Pin 12 (Pi Pico)s
    myServo.write(startAngle);
    Serial.println("System gestartet. Servo auf 0 Grad.");
    delay(1000);
}

void moveServoSmooth(int fromAngle, int toAngle) {
    if (fromAngle < toAngle) {
        for (int pos = fromAngle; pos <= toAngle; pos++) {
            myServo.write(pos);
            delay(servoSpeed);
        }
    } else {
        for (int pos = fromAngle; pos >= toAngle; pos--) {
            myServo.write(pos);
            delay(servoSpeed);
        }
    }
}

void loop() {
    moveServoSmooth(startAngle, targetAngle);
    delay(1000);
    moveServoSmooth(targetAngle, startAngle);
    delay(1000);
}
