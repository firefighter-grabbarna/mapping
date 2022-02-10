// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include "main.h"

const int PWM_PIN = A0;
const int standStill = 1500; // Controller in middle position
const int BAUD_RATE = 9600;

AF_DCMotor RightBackWheel(3);
AF_DCMotor LeftBackWheel(2);
AF_DCMotor LeftFrontWheel(4);
AF_DCMotor RightFrontWheel(1);

void setup() {
   //115200
   Serial.begin(BAUD_RATE);
   Serial.println("Motor test!");

   // turn on motor dont crash code
   RightBackWheel.run(RELEASE);
   LeftBackWheel.run(RELEASE);
   LeftFrontWheel.run(RELEASE);
   RightFrontWheel.run(RELEASE);

}

void loop() {

   int pwmValue = pulseIn(PWM_PIN, HIGH); // Read controller
   Serial.println(pwmValue);
   //Serial.println("\n");
   setSpeed(convertSpeed(pwmValue));
   
   switch(getDir(pwmValue)){
      case MOVE_STOP:
         motorStop();
         break;
      case MOVE_FORWARD:
         moveForward();
         break;
      case MOVE_BACKWARD:
         moveBackward();
         break;
   }
   /*
   if (((standStill - threshold) < pwmValue) && ((standStill + threshold) > pwmValue) ){
      motorStop();
   }
   else if (movingForward){
      moveForward();
   }
   else if (movingBackward){
      moveBackward();
   }
   */
}

directions getDir(int pwmValue){
   int threshold = 50;
   if (((standStill - threshold) < pwmValue) && ((standStill + threshold) > pwmValue)) return MOVE_STOP;
   if (pwmValue > standStill) return MOVE_FORWARD;
   if (pwmValue < standStill) return MOVE_BACKWARD;
   return MOVE_STOP;
}

int convertSpeed(int speed){
   return abs(standStill - speed) / 2;
}

void setSpeed(int speed){
   const float SCALE_LEFT_WHEELS = 0.9;

   RightBackWheel.setSpeed(speed);
   LeftBackWheel.setSpeed(speed * SCALE_LEFT_WHEELS);
   LeftFrontWheel.setSpeed(speed * SCALE_LEFT_WHEELS);
   RightFrontWheel.setSpeed(speed);

}

void moveForward()
{
   LeftFrontWheel.run(FORWARD);
   LeftBackWheel.run(FORWARD);
   
   RightFrontWheel.run(FORWARD); 
   RightBackWheel.run(FORWARD);
  
}
void moveBackward() {

   LeftFrontWheel.run(BACKWARD);
   LeftBackWheel.run(BACKWARD);
   
   RightFrontWheel.run(BACKWARD);
   RightBackWheel.run(BACKWARD);
   
}
void moveSidewaysRight() {

   LeftFrontWheel.setSpeed(120);
   LeftBackWheel.setSpeed(120);
  
   RightBackWheel.setSpeed(120);
   RightFrontWheel.setSpeed(120);

   LeftFrontWheel.run(FORWARD);
   LeftBackWheel.run(BACKWARD);

   RightFrontWheel.run(FORWARD);
   RightBackWheel.run(BACKWARD);
   
}
void moveSidewaysLeft() {

   LeftFrontWheel.setSpeed(120);
   LeftBackWheel.setSpeed(100);

   RightFrontWheel.setSpeed(100);
   RightBackWheel.setSpeed(120);
   
  
   LeftFrontWheel.run(BACKWARD);
   LeftBackWheel.run(FORWARD);
   
   RightFrontWheel.run(BACKWARD);
   RightBackWheel.run(FORWARD);
   
}
void rotateLeft() {
  
   LeftFrontWheel.run(BACKWARD);
   LeftBackWheel.run(BACKWARD);
   
   RightFrontWheel.run(FORWARD);
   RightBackWheel.run(FORWARD);
   
}
void rotateRight() {
  
   LeftFrontWheel.run(FORWARD);
   LeftBackWheel.run(FORWARD);
   
   RightFrontWheel.run(BACKWARD);
   RightBackWheel.run(BACKWARD);
   
}
void moveRightForward() {
  
   LeftFrontWheel.run(FORWARD);
  // LeftBackWheel.run(FORWARD);
   
 //  RightFrontWheel.run(BACKWARD);
   RightBackWheel.run(BACKWARD);
   
}
void moveRightBackward() {
  
  // LeftFrontWheel.run(FORWARD);
   LeftBackWheel.run(BACKWARD);
   
   RightFrontWheel.run(BACKWARD);
 //  RightBackWheel.run(BACKWARD);
   
}
void moveLeftForward() {
  // LeftFrontWheel.run(FORWARD);
   LeftBackWheel.run(FORWARD);
   
   RightFrontWheel.run(FORWARD);
 //  RightBackWheel.run(BACKWARD);
}
void moveLeftBackward() {
  
   LeftFrontWheel.run(BACKWARD);
  // LeftBackWheel.run(FORWARD);
   
 //  RightFrontWheel.run(FORWARD);
   RightBackWheel.run(BACKWARD);
}
void motorStop(){
  
   LeftFrontWheel.run(RELEASE);
   LeftBackWheel.run(RELEASE);
   
   RightFrontWheel.run(RELEASE);
   RightBackWheel.run(RELEASE);
}



