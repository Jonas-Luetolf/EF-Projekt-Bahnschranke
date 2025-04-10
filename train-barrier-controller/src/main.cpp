#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>

Servo myServo;

#define SLAVE_ADDR 0x03

#define SERVO_SPEED 15 // 15 als Defaultwert ist gut für Schranke
#define START_ANGLE 0
#define TARGET_ANGLE 180

#define SDA_PIN 4
#define SCL_PIN 5

volatile uint8_t last_command_code = 0;


void moveServoSmooth(int fromAngle, int toAngle) {
    if (fromAngle < toAngle) {
        for (int pos = fromAngle; pos <= toAngle; pos++) {
            myServo.write(pos);
            delay(SERVO_SPEED);
        }
    } else {
        for (int pos = fromAngle; pos >= toAngle; pos--) {
            myServo.write(pos);
            delay(SERVO_SPEED);
        }
    }
}

bool closeBarrier() {
    moveServoSmooth(START_ANGLE, TARGET_ANGLE);
    return true;
}

bool openBarrier() {
    moveServoSmooth(TARGET_ANGLE, START_ANGLE);
    return true;
}

void onReceive(int numBytes){
    if (Wire.available()) {
      last_command_code = Wire.read();
      Serial.print("INFO: got command ");
      Serial.println(last_command_code, HEX);
    }
  }
  
  
  /**
   * @brief Responds to requests from the I2C master.
   * 
   * This function is called when the master requests data from the slave.
   * If no valid command was received, it returns -1 as an error indicator.
   * Otherwise, it processes the last received command and responds with the appropriate data.
   */
  void onRequest() {
    if (last_command_code == 0){
      Serial.println("WARNING: Request without command");
      Wire.write(-1);
    }
  
    else {
      switch (last_command_code)
      {
      // 0x01 ask direction
      case 0x01:
      Serial.print("INFO: executed command ");
      Serial.println(last_command_code, HEX);
      closeBarrier();
      Wire.write(0);
      break;
    
    case 0x02:
      Serial.print("INFO: executed command ");
      Serial.println(last_command_code, HEX);
      openBarrier();
      Wire.write(0);
      break;

    
      case 0x03: {
        // Serial debug info executed command
        Serial.print("INFO: executed command");
        Serial.println(last_command_code, HEX);
  
        // Schranke Status abfragen
        uint8_t state = 0;
        Wire.write(state);
        break;
      }


      default:
        Wire.write(-1);
      }
      last_command_code = 0;
    }
  }

  void setup() {
    Serial.begin(115200);
    myServo.attach(9); // GP9 -> Pin 12 (Pi Pico)s
    myServo.write(START_ANGLE);
    Wire.setSDA(SDA_PIN);
    Wire.setSCL(SCL_PIN);
    Wire.begin(SLAVE_ADDR);
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
    Serial.println("System gestartet. Servo auf 0 Grad.");
    delay(1000);
}

void loop() {
    closeBarrier();
    delay(1000);
    openBarrier();
    delay(1000);
}
