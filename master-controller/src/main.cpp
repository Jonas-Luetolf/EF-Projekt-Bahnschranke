#include <Arduino.h>
#include <Wire.h>

// I2C addresses
#define LIGHTBARRIER1 0x01
#define LIGHTBARRIER2 0x02

#define TRAINBARRIER 0x03

#define SIGNAL1 0x04
#define SIGNAL2 0x05

// I2C Pins
#define SDA_PIN 21
#define SCL_PIN 22

// Signalbilder
#define SIG0 0x01 // Rot: Halt
#define SIG1 0x03 // Grün: freie Fahrt
#define SIG2 0x02 // Grün + Gelb: max. 40 km/h

// Interrupt Pins
#define LIGHTBARRIER1_CALL_PIN 13
#define LIGHTBARRIER2_CALL_PIN 12

volatile bool eventLightBarrier1 = false;
volatile bool eventLightBarrier2 = false;

// Light Barrier Functions

int askDirectionFromBarrier(int barrierAddr) {
  int direction = 1;
  Wire.beginTransmission(barrierAddr);
  Wire.write(0x01);
  Wire.endTransmission();

  delay(100);

  Wire.requestFrom(barrierAddr, 1);
  while (Wire.available()) {
      direction = Wire.read();
      Serial.print("INFO: Received direction from Light Barrier Controller ");
      Serial.print(barrierAddr);
      Serial.print(": ");
      Serial.println(direction);
  }

  return direction;
}


// Train Barrier Functions

void sendBarrierMoveCommand(int barrierAddr, int command){
  Wire.beginTransmission(barrierAddr);
  Wire.write(command);
  Wire.endTransmission();

  delay(100);

  Wire.requestFrom(barrierAddr, 1);
  while (Wire.available()) {
    int c = Wire.read();
    Serial.print("INFO: Received from Barrier-Controller ");
    Serial.print(barrierAddr);
    Serial.print(": ");
    Serial.println(c);
  }
}


void closeBarrier(int barrierAddr){
  sendBarrierMoveCommand(barrierAddr, 0x01);
}


void openBarrier(int barrierAddr){
  sendBarrierMoveCommand(barrierAddr, 0x02);
}


bool readBarrier(int barrierAddr){
  Wire.beginTransmission(barrierAddr);
  Wire.write(0x03);
  Wire.endTransmission();

  delay(100);

  bool state = false;
  Wire.requestFrom(barrierAddr, 1);
  while (Wire.available()) {
    int c = Wire.read();
    Serial.print("INFO: Received state from Train Barrier-Controller ");
    Serial.print(barrierAddr);
    Serial.print(": ");
    Serial.println(c);
    state = c;
  } 
  return state;
}


// Signal Functions

void setTrainSignal(int signalAddr, int colorNum) {
  Wire.beginTransmission(signalAddr);
  Wire.write(colorNum);
  Wire.endTransmission();

  delay(100);

  Wire.requestFrom(signalAddr, 1);
  while (Wire.available()) {
      int c = Wire.read();
      Serial.print("INFO: Received from Signal-Controller ");
      Serial.print(signalAddr);
      Serial.print(": ");
      Serial.println(c);
  }
}


// Interrupts
void IRAM_ATTR handelCallLightBarrier1() {
  eventLightBarrier1 = true;
}


void IRAM_ATTR handelCallLightBarrier2() {
  eventLightBarrier2 = true;
}


void handleEvent(int direction, int side){
  switch (direction)
  {
  case 1:
    {
    closeBarrier(TRAINBARRIER);
    while (!readBarrier(TRAINBARRIER)){
      delay(200);
    }
    
    switch (side)
    {
    case 1:
      setTrainSignal(SIGNAL1, SIG1);
      break;
    
    case 2:
      setTrainSignal(SIGNAL2, SIG1);
      break;
    }
    break;
  }  
  case 2:
  {
    setTrainSignal(SIGNAL1, SIG0);
    setTrainSignal(SIGNAL2, SIG0);
    openBarrier(TRAINBARRIER);
    break;
  }
}
}


// Core 0
void setup() {
  attachInterrupt(LIGHTBARRIER1_CALL_PIN, handelCallLightBarrier1, RISING);
  attachInterrupt(LIGHTBARRIER2_CALL_PIN, handelCallLightBarrier2, RISING);

  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
}

void loop() {
  if (eventLightBarrier1){
    int direction = askDirectionFromBarrier(LIGHTBARRIER1);
    handleEvent(direction, 1);
    readBarrier(TRAINBARRIER);
    eventLightBarrier1 = false;
  }
  
  if (eventLightBarrier2){
    int direction = askDirectionFromBarrier(LIGHTBARRIER2);
    handleEvent(direction, 2);
    eventLightBarrier2 = false;    
  }
  delay(100);
}