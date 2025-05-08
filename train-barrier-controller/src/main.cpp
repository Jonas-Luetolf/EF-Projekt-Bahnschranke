#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>


#define SLAVE_ADDR 0x03

// master communication pins
#define SDA_PIN 4
#define SCL_PIN 5

// servo settings
#define SERVO_SPEED 15
#define START_ANGLE 0
#define TARGET_ANGLE 180
#define SERVO_PIN 6

#define BARRIER_STATE_PIN 7

// global variables
volatile uint8_t last_command_code = 0;
volatile uint8_t pending_command = 0;
Servo barrierServo;

// Core 0 I2C communication and handling of servo actions


/**
 * @brief Smoothly moves the servo from one angle to another.
 * 
 * Moves the servo from fromAngle to toAngle in small steps with a delay,
 * creating a smooth transition effect.
 * 
 * @param fromAngle Starting angle of the servo.
 * @param toAngle   Target angle to move the servo to.
 */
void moveServoSmooth(int fromAngle, int toAngle) {
  int step = (fromAngle < toAngle) ? 1 : -1;
  for (int pos = fromAngle; pos != toAngle + step; pos += step) {
      barrierServo.write(pos);
      delay(SERVO_SPEED);
  }
}


void closeBarrier() {
    moveServoSmooth(START_ANGLE, TARGET_ANGLE);
}


void openBarrier() {
    moveServoSmooth(TARGET_ANGLE, START_ANGLE);
}


void onReceive(int numBytes){
    if (Wire.available()) {
      last_command_code = Wire.read();
      Serial.print("INFO: got command ");
      Serial.println(last_command_code, HEX);
    }
  }
  
  
  /**
   * @brief Responds to requests from the I2C master and executes the comand set in last_command_code.
   * 
   * This function is called when the master requests data from the slave.
   * If no valid command was received, it returns -1 as an error indicator.
   * Otherwise, it processes the last received command and responds with the appropriate data.
   */
  void onRequest() {
    if (last_command_code == 0){
      Serial.println("WARNING: Request without command");
      Wire.write(1);
    }
  
    else {
      switch (last_command_code)
      {
      // 0x01 close
      // 0x02 open
      case 0x01:
      case 0x02:
        Serial.print("INFO: executed command ");
        Serial.println(last_command_code, HEX);
        pending_command = last_command_code;
        Wire.write(0);
        break;

      // check barrier state
      case 0x03: {
        // Serial debug info executed command
        Serial.print("INFO: executed command");
        Serial.println(last_command_code, HEX);
  
        uint8_t state = digitalRead(BARRIER_STATE_PIN);

        Serial.print("INFO: send barrier stat:");
        Serial.print(state);
        Serial.println(" to master");

        Wire.write(state);
        break;
      }

      default:
        Wire.write(1);
      }
      last_command_code = 0;
    }
  }


void setup() {
  Serial.begin(115200);

  // init I2C master communication
  Wire.setSDA(SDA_PIN);
  Wire.setSCL(SCL_PIN);
  Wire.begin(SLAVE_ADDR);

  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);

  // init Servo
  barrierServo.attach(SERVO_PIN);
  barrierServo.write(START_ANGLE);

  pinMode(BARRIER_STATE_PIN, INPUT);
  Serial.print("INFO: init Servo; moved to");
  Serial.print(START_ANGLE);
  Serial.println("°");
  delay(500);
}

void loop() {
  // check if an command has to be executed
  if (pending_command != 0) {
    if (pending_command == 0x01) {
      closeBarrier();
    } else if (pending_command == 0x02) {
      openBarrier();
    }
    pending_command = 0;
  }
  
  delay(100);
}