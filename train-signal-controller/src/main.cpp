#include <Arduino.h>
#include <Wire.h>

#define SLAVE_ADDR 0x04
#define SDA_PIN 0
#define SCL_PIN 1

volatile uint8_t last_command_code = 0;


void setup() {
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
/*
    Serial.begin(115200);

    Wire.setSDA(SDA_PIN);
    Wire.setSCL(SCL_PIN);
    Wire.begin(SLAVE_ADDR);
  
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
*/


}

void triggerRed() {
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(2, HIGH);
    return;
}

void triggerYellow() {
    digitalWrite(2, LOW);
    digitalWrite(4, LOW);
    digitalWrite(3, HIGH);
    return;
}

void triggerGreen() {
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, HIGH);
    return;
}

void onReceive(int numBytes){
    if (Wire.available()) {
      last_command_code = Wire.read();
      Serial.print("INFO: got command ");
      Serial.println(last_command_code, HEX);
    }
  }
  

void onRequest() {
    if (last_command_code == 0){
      Serial.println("WARNING: Request without command");
      Wire.write(-1);
    }
  
    else {
      switch (last_command_code)
      {
      // 0x01 close
      case 0x01:
      Serial.print("INFO: executed command ");
      Serial.println(last_command_code, HEX);
      triggerRed();
      // closeBarrier();
      Wire.write(0);
      break;
    
      // 0x02 open
      case 0x02:
        Serial.print("INFO: executed command ");
        Serial.println(last_command_code, HEX);
        triggerYellow();
        //openBarrier();
        Wire.write(0);
        break;

      // check barrier state
      case 0x03: {
        // Serial debug info executed command
        Serial.print("INFO: executed command");
        Serial.println(last_command_code, HEX);
  
        triggerGreen();

        Wire.write(0);
        break;
      }

      default:
        Wire.write(-1);
      }
      last_command_code = 0;
    }
  }

void loop() {
    triggerGreen();
    delay(100);
    triggerYellow();
    delay(100);
    triggerRed();
    delay(100);
}