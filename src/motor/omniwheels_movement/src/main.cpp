#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// TODO: The wheels will need to be defined corretly
Adafruit_DCMotor *LeftFrontWheel  = AFMS.getMotor(1);
Adafruit_DCMotor *RightFrontWheel   = AFMS.getMotor(2); 
Adafruit_DCMotor *LeftBackWheel = AFMS.getMotor(3);
Adafruit_DCMotor *RightBackWheel  = AFMS.getMotor(4);

int defaultSpeed = 60;

void setup() {

  Serial.begin(115200);
  // put your setup code here, to run once:
  AFMS.begin();
  
  LeftFrontWheel->setSpeed(defaultSpeed);
  LeftBackWheel->setSpeed(defaultSpeed);
  
  RightBackWheel->setSpeed(defaultSpeed);
  RightFrontWheel->setSpeed(defaultSpeed);

  delay(20);
  
}

void loop() {
  
  // put your main code here, to run repeatedly:

}
void moveForward()
{
   LeftFrontWheel->run(FORWARD);
   LeftBackWheel->run(FORWARD);
   
   RightFrontWheel->run(FORWARD); 
   RightBackWheel->run(FORWARD);
  
}
void moveBackward() {
  
   LeftFrontWheel->run(BACKWARD);
   LeftBackWheel->run(BACKWARD);
   
   RightFrontWheel->run(BACKWARD);
   RightBackWheel->run(BACKWARD);
   
}
void moveSidewaysRight() {

   LeftFrontWheel->setSpeed(120);
   LeftBackWheel->setSpeed(120);
  
   RightBackWheel->setSpeed(120);
   RightFrontWheel->setSpeed(120);

   LeftFrontWheel->run(FORWARD);
   LeftBackWheel->run(BACKWARD);

   RightFrontWheel->run(FORWARD);
   RightBackWheel->run(BACKWARD);
   
}
void moveSidewaysLeft() {

   LeftFrontWheel->setSpeed(120);
   LeftBackWheel->setSpeed(100);

   RightFrontWheel->setSpeed(100);
   RightBackWheel->setSpeed(120);
   
  
   LeftFrontWheel->run(BACKWARD);
   LeftBackWheel->run(FORWARD);
   
   RightFrontWheel->run(BACKWARD);
   RightBackWheel->run(FORWARD);
   
}
void rotateLeft() {
  
   LeftFrontWheel->run(BACKWARD);
   LeftBackWheel->run(BACKWARD);
   
   RightFrontWheel->run(FORWARD);
   RightBackWheel->run(FORWARD);
   
}
void rotateRight() {
  
   LeftFrontWheel->run(FORWARD);
   LeftBackWheel->run(FORWARD);
   
   RightFrontWheel->run(BACKWARD);
   RightBackWheel->run(BACKWARD);
   
}
void moveRightForward() {
  
   LeftFrontWheel->run(FORWARD);
  // LeftBackWheel->run(FORWARD);
   
 //  RightFrontWheel->run(BACKWARD);
   RightBackWheel->run(BACKWARD);
   
}
void moveRightBackward() {
  
  // LeftFrontWheel->run(FORWARD);
   LeftBackWheel->run(BACKWARD);
   
   RightFrontWheel->run(BACKWARD);
 //  RightBackWheel->run(BACKWARD);
   
}
void moveLeftForward() {
  // LeftFrontWheel->run(FORWARD);
   LeftBackWheel->run(FORWARD);
   
   RightFrontWheel->run(FORWARD);
 //  RightBackWheel->run(BACKWARD);
}
void moveLeftBackward() {
  
   LeftFrontWheel->run(BACKWARD);
  // LeftBackWheel->run(FORWARD);
   
 //  RightFrontWheel->run(FORWARD);
   RightBackWheel->run(BACKWARD);
}
void motorStop(){
  
   LeftFrontWheel->run(RELEASE);
   LeftBackWheel->run(RELEASE);
   
   RightFrontWheel->run(RELEASE);
   RightBackWheel->run(RELEASE);
}

void setSpeed()
{
   LeftFrontWheel->setSpeed(defaultSpeed);
  LeftBackWheel->setSpeed(defaultSpeed);
  
  RightBackWheel->setSpeed(defaultSpeed);
  RightFrontWheel->setSpeed(defaultSpeed);
}