#include <Arduino.h>
#include <Wire.h>

#define SLAVE_ADDR 0x04
#define SDA_PIN 0
#define SCL_PIN 1
#define RED_PIN 2
#define YELLOW_PIN 3
#define GREEN_PIN 4

volatile uint8_t last_command_code = 0;




void triggerRed() {
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(RED_PIN, HIGH);
    return;
}

void triggerYellow() {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(YELLOW_PIN, HIGH);
    return;
}

void triggerGreen() {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
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
        Wire.write(1);
      }
      last_command_code = 0;
    }
  }

  void setup() {
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);

    Serial.begin(115200);

    Wire.setSDA(SDA_PIN);
    Wire.setSCL(SCL_PIN);
    Wire.begin(SLAVE_ADDR);
  
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);


}

void loop() {
    delay(100);
    /* 
    triggerGreen();
    delay(20);
    triggerYellow();
    delay(20);
    triggerRed();
    delay(20);

    */
}