#include <Arduino.h>
#include <Wire.h>

// I2C configs
#define SLAVE_ADDR 0x05
#define SDA_PIN 4
#define SCL_PIN 5


// Signal Pins
#define RED_PIN 14
#define YELLOW_PIN 15
#define GREEN_PIN 26

volatile uint8_t last_command_code = 0;


void trigger0() {
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(RED_PIN, HIGH);
}


void trigger2() {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(YELLOW_PIN, HIGH);
}


void trigger1() {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
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
      // 0x01 Red: Halt
      case 0x01:
      Serial.print("INFO: executed command ");
      Serial.println(last_command_code, HEX);

      trigger0();
      
      Wire.write(0);
      break;
    
      // 0x02 Green + Yellow: freie Fahrt max. 40 km/h
      case 0x02:
        Serial.print("INFO: executed command ");
        Serial.println(last_command_code, HEX);

        trigger2();
  
        
        Wire.write(0);
        break;
      
      // 0x03 Green: freie Fahrt 
      case 0x03: {
        // Serial debug info executed command
        Serial.print("INFO: executed command ");
        Serial.println(last_command_code, HEX);
  
        trigger1();

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
    pinMode(RED_PIN, OUTPUT);
    pinMode(YELLOW_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);

    Serial.begin(115200);

    Wire.setSDA(SDA_PIN);
    Wire.setSCL(SCL_PIN);

    Wire.begin(SLAVE_ADDR);
  
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);

    Serial.print("INFO: init I2C slave on Addr: ");
    Serial.println(SLAVE_ADDR, HEX);
}


void loop() {
    delay(100);
}