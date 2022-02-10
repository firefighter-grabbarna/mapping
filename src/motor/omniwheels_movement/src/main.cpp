// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include <AFMotor.h>
#include <Arduino.h>

const int PWM_PIN = A0;
const int standStill = 1500;

AF_DCMotor RightBackWheel(3);
AF_DCMotor LeftBackWheel(2);
AF_DCMotor LeftFrontWheel(4);
AF_DCMotor RightFrontWheel(1);

void setup() {
   //115200
   Serial.begin(9600);
   Serial.println("Motor test!");

   // turn on motor dont crash code
   RightBackWheel.run(RELEASE);
   LeftBackWheel.run(RELEASE);
   LeftFrontWheel.run(RELEASE);
   RightFrontWheel.run(RELEASE);

}
int convertSpeed(int speed){
   int final = 0;
   if (speed < standStill){
      final = standStill - speed;
   }
   else{
      final = speed - standStill;
   }

   return (final)/2;
}

void setSpeed(int speed){

   RightBackWheel.setSpeed(speed);
   LeftBackWheel.setSpeed(speed);
   LeftFrontWheel.setSpeed(speed);
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
void loop() {

   int pwmValue = pulseIn(PWM_PIN, HIGH);
   Serial.println(pwmValue);
   //Serial.println("\n");
   bool movingForward = pwmValue > standStill;
   bool movingBackward = pwmValue < standStill;
   int threshold = 50;
   
   if (((standStill - threshold) < pwmValue) && ((standStill + threshold) > pwmValue) ){
      motorStop();
   }
   else if (movingForward){
      setSpeed(convertSpeed(pwmValue));
      moveForward();
   }
   else if (movingBackward){
      setSpeed(convertSpeed(pwmValue));
      moveBackward();
   }
}


