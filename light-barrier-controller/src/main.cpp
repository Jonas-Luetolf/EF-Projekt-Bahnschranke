#include <Arduino.h>
#include <Wire.h>

#define SLAVE_ADDR 0x01

// light barrier input pins
#define LIGHT_BARRIER_1 26
#define LIGHT_BARRIER_2 27
#define LIGHT_THRESHOLD 15

// master communication pins
#define MASTER_CALL_PIN 2
#define SDA_PIN 4
#define SCL_PIN 5

// global variables
volatile uint8_t last_command_code = 0;
volatile uint8_t last_trigger_direction = 0;

// Core 0 I2C communication


/**
 * @brief Sends a signal to the master to indicate that an event has occurred.
 * 
 * This function sets the MASTER_CALL_PIN high for 10 milliseconds 
 * and then sets it low again, effectively sending a pulse signal 
 * to notify the master device.
 */
void callMaster() {
  digitalWrite(MASTER_CALL_PIN, HIGH);
  delay(10); //TODO: check delay
  digitalWrite(MASTER_CALL_PIN, LOW);
}


/**
 * @brief Handles incoming data from the I2C master.
 * 
 * This function is triggered when the master sends data to the slave. 
 * It reads the received command and stores it in the `last_command_code` variable.
 * 
 * @param numBytes The number of bytes received from the master.
 */
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
    Wire.write(-1);
  }

  else {
    switch (last_command_code)
    {
    // 0x01 ask direction
    case 0x01:
      // Serial debug info executed command
      Serial.print("INFO: executed command");
      Serial.println(last_command_code, HEX);

      if (last_trigger_direction != 0){
        Serial.print("INFO: send ");
        Serial.print(last_trigger_direction);
        Serial.println("back to master");
      }

      else {
        Serial.println("WARNING: Master called direction but no event was recognized. Send Error code -1 back to master.");
      }

      Wire.write(last_trigger_direction);

      // reset last trigger direction to no Trigger / Error
      last_trigger_direction = 0;
      break;
    
    default:
      Wire.write(-1);
    }
    last_command_code = 0;
  }
}


void setup(){
  Serial.begin(11520000);
  
  // init i2c slave
  Wire.setSDA(SDA_PIN);
  Wire.setSCL(SCL_PIN);
  Wire.begin(SLAVE_ADDR);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
}


void loop() {
  delay(100);
}

// Core 1 Handling of light barriers

// store the time of last trigger; -1 not unhandled
long lastTrigger1 = -1;
long lastTrigger2 = -1;


/**
 * @brief Checks if the light beam is interrupted at the given analog sensor.
 *
 * @param analogPin Analog pin connected to the phototransistor.
 * @return true if the sensor value is below LIGHT_THRESHOLD (beam interrupted), false otherwise.
 */
bool isTriggered(int analogPin) {
  return analogRead(analogPin) < LIGHT_THRESHOLD;
}


void setup1() {
  pinMode(MASTER_CALL_PIN, OUTPUT); // Interrupt Pin for Master-Controller
}


void loop1() {
  // check light barrier 1
  if (isTriggered(LIGHT_BARRIER_1) && lastTrigger1 == -1) {
    Serial.println("INFO: light barrier 1 triggered");
    lastTrigger1 = millis();
  }

  // check light barrier 2
  if (isTriggered(LIGHT_BARRIER_2) && lastTrigger2 == -1) {
    Serial.println("INFO: light barrier 2 triggered");
    lastTrigger2 = millis();
  }

  // check if both light barriers got triggered
  if (lastTrigger1 != -1 and lastTrigger2 != -1){

    // get direction
    long diff = lastTrigger1 - lastTrigger2;

    if (diff > 0) { // 1 -> 2
      last_trigger_direction = 1;
    }
    else {
      if (diff < 0) { // 2 -> 1
      last_trigger_direction = 2;
      }
      else {
        Serial.println("ERROR: both trigger times equal, failed to get direction. Direction set to default 0.");
        last_trigger_direction = 0;
      }
    }
    Serial.print("INFO: Trigger in direction: ");
    Serial.println(last_trigger_direction);
    callMaster();

    // wait until both light barriers are rested
    while (isTriggered(LIGHT_BARRIER_1) || isTriggered(LIGHT_BARRIER_2)) {
      delay(1000);
    }

    // reset last trigger times to no trigger
    lastTrigger1 = -1;
    lastTrigger2 = -1;   
  }

  delay(100);
}