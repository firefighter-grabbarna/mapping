// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include "main.h"

const int PWM_PIN = A0;
const int standStill = 1500; // Controller in middle position
const int BAUD_RATE = 9600;



const int RIGHT_FRONT_WHEEL = 1;
const int LEFT_BACK_WHEEL = 2;
const int RIGHT_BACK_WHEEL = 3;
const int LEFT_FRONT_WHEEL = 4;

const int SPEED_THRESHOLD = 0;


AF_DCMotor RightFrontWheel(1);
AF_DCMotor LeftBackWheel(2);
AF_DCMotor RightBackWheel(3);
AF_DCMotor LeftFrontWheel(4);


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
/*

calculates the wheel speed for moving in a perticular direction
param - omega is the angular speed
param - x is the speed in x-direction
param - y is the speed in y-direction

NOTE: since the matrix is of dimension 4 an arbitrary choice of u1,u2,u3,u4 can cause skidding
      (it is belived to be what it means)
      These vidoes are of concern:
      How the speed is calculated:
https://www.youtube.com/watch?v=NcOT9hOsceE

      This talks about how to limit of our speed needs to be taken into account:
      (one could just do low enought speed and this would probably not be a problem)
https://www.youtube.com/watch?v=B1K-ti5Lqjc

*/

void getWheelSpeeds(double x, double y, double omegaZ, double (&speedVector)[4]){

   

   speedVector[LEFT_FRONT_WHEEL-1] = (x + y + omegaZ ); // 1 in coppa // left front 
   speedVector[LEFT_BACK_WHEEL-1] = ( x - y + omegaZ ); // 2 in coppa // left back  
   speedVector[RIGHT_BACK_WHEEL-1] = ( x + y - omegaZ ); // 3 in coppa // right back  ? 
   speedVector[RIGHT_FRONT_WHEEL-1] = ( x - y -omegaZ ); // 4 in coppa // right front ?
}
/*

   Given a speed and a wheel 
   sets the correct direction for the wheel

   A negative speed means backwards
   A positive speed means fortwads
*/

void setSpeedAndDirection(double speed, AF_DCMotor &wheel){ 
   int int_speed = (int) speed;
   
   if (speed >= SPEED_THRESHOLD){
      wheel.run(FORWARD);
      wheel.setSpeed(int_speed);
   }
   else if (speed <= -SPEED_THRESHOLD){
      wheel.run(BACKWARD);
      wheel.setSpeed(abs(int_speed));
   }

}

/*

   Used in for example a for- loop where an integer makes an action on the 
   corresponding wheel
*/

void setWheelSpeed(int wheel_case, double speed2){
   int speed = (int) speed2;
   switch (wheel_case)
   {
   case LEFT_BACK_WHEEL:
      setSpeedAndDirection(speed, LeftBackWheel); 
      break;
   case RIGHT_BACK_WHEEL:
      setSpeedAndDirection(speed, RightBackWheel);
      break;
   case LEFT_FRONT_WHEEL:
      setSpeedAndDirection(speed, LeftFrontWheel);
      break;
   case RIGHT_FRONT_WHEEL:
      setSpeedAndDirection(speed, RightFrontWheel);
      break;
   }
}
/*
   Sets the speed of each wheel to the elements of u
*/



void setWheelSpeed(double (&u)[4]){
   // make sure the wheels are mapped to the correct for this to work!
   
     /*  This should work so the loop should also work test it
      setWheelSpeed(LEFT_FRONT_WHEEL, u[LEFT_FRONT_WHEEL-1]);
      setWheelSpeed(LEFT_BACK_WHEEL, u[LEFT_BACK_WHEEL - 1]);
      setWheelSpeed(RIGHT_BACK_WHEEL, u[RIGHT_BACK_WHEEL- 1]);
      setWheelSpeed(RIGHT_FRONT_WHEEL, u[RIGHT_FRONT_WHEEL - 1]);
         */
   int i;
   for (i = 0; i <= 3; i++){
      setWheelSpeed(i + 1, u[i]);
      Serial.println(u[i]);
   }
}


void loop() {
   // ---------------------------
   // this code is untested and therefore commented until it can be tested
   // DO NOT REMOVE as i this is to be tested
   // -----------------------------
   
   double speedVector[4];
   // this should move the car in x-direction
   getWheelSpeeds(100, 0, 0, speedVector);
   setWheelSpeed(speedVector);
   delay(2000);
   // this should move the car in y-direction
   getWheelSpeeds(0, 100, 0, speedVector);
   setWheelSpeed(speedVector);
   delay(2000);
   // this should spin the car in a circle
   getWheelSpeeds(0, 0, 100, speedVector);
   setWheelSpeed(speedVector);
   delay(2000);
   // this should spin the car in a circle the other direction
   getWheelSpeeds(0, 0, -100, speedVector);
   setWheelSpeed(speedVector);
   delay(2000);
   // this should move diagonal
   getWheelSpeeds(100, 100, 0, speedVector);
   setWheelSpeed(speedVector);
   delay(2000);
   // this should spin the car while moving y - direction
   getWheelSpeeds(0, 100, 100, speedVector);
   setWheelSpeed(speedVector);
   delay(2000);
   // this should spin the car while moving x and y - direction
   getWheelSpeeds(100, 100, 100, speedVector);
   setWheelSpeed(speedVector);


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



