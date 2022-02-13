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

const int SPEED_THRESHOLD = 10;


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
   /* 
   l is the distance from the middle of the base to the middle of the wheel
   w is the distance from the wheel to the middle of the base.
   These distances are not diagonal, only the straight line on the x or y axis
   */ 
   double l = -1; // TODO: update these to the correct value 
   double w = -1;
   double r = -1; // the radius of the wheel
   /*
   This is the resulting vector from matrix mutliplication of
           
      u1    -l-w   1  -1     
      u2  =  l+w   1   1  *   w = H(0)*vb
      u3     l+w   1  -1      x
      u4    -l-w   1   1      y



   found in coppeliaSim a simulation program
      function setMovement(forwBackVel,leftRightVel,rotVel)
    -- Apply the desired wheel velocities:
    sim.setJointTargetVelocity(wheelJoints[1],-forwBackVel-leftRightVel-rotVel)
    sim.setJointTargetVelocity(wheelJoints[2],-forwBackVel+leftRightVel-rotVel)
    sim.setJointTargetVelocity(wheelJoints[3],-forwBackVel-leftRightVel+rotVel)
    sim.setJointTargetVelocity(wheelJoints[4],-forwBackVel+leftRightVel+rotVel)
end
                          
   */
   // this is the same as the commented code but reversed minus tecken. forwBackVel = x leftRightVel = y rotVel = omegaZ
   double u1 = (1/r) * ( (-l - w) * omegaZ + x - y); // 4 in coppa
   double u2 = (1/r) * ( (l + w) * omegaZ + x + y); // 1 in coppa
   double u3 = (1/r) * ( (l + w) * omegaZ + x - y); // 2 in coppa
   double u4 = (1/r) * ( (-l - w) * omegaZ + x + y); // 3 in coppa
   // the wheels should be mapped correctly, however test this to make sure
   speedVector[0] = u2; // right front
   speedVector[1] = u4; // left back
   speedVector[2] = u3; // right back
   speedVector[3] = u1; // left front
}
/*

   Given a speed and a wheel 
   sets the correct direction for the wheel

   A negative speed means backwards
   A positive speed means fortwads
*/

void setSpeedAndDirection(int speed, AF_DCMotor &wheel){ 
   if (speed >= SPEED_THRESHOLD){
      wheel.run(FORWARD);
      wheel.setSpeed(speed);
   }
   else if (speed <= -SPEED_THRESHOLD){
      wheel.run(BACKWARD);
      wheel.setSpeed(abs(speed));
   }

}

/*

   Used in for example a for- loop where an integer makes an action on the 
   corresponding wheel
*/

void setWheelSpeed(int wheel_case, int speed){
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
   for (int i = 1; i == sizeof(u) ; i++){
      setWheelSpeed(i, u[i]);
   }
}


void loop() {
   // ---------------------------
   // this code is untested and therefore commented until it can be tested
   // DO NOT REMOVE as i this is to be tested
   // -----------------------------
   /*
   
   double speedVector[4];
   // this should move the car in x-direction
   getWheelSpeeds(100, 0, 0, speedVector);
   setWheelSpeed(speedVector);
   // this should move the car in y-direction
   getWheelSpeeds(0, 100, 0, speedVector);
   setWheelSpeed(speedVector);
   // this should spin the car in a circle
   getWheelSpeeds(0, 0, 50, speedVector);
   setWheelSpeed(speedVector);
   // this should spin the car in a circle the other direction
   getWheelSpeeds(0, 0, -50, speedVector);
   setWheelSpeed(speedVector);
   // this should move diagonal
   getWheelSpeeds(50, 50, 0, speedVector);
   setWheelSpeed(speedVector);
   // this should spin the car while moving y - direction
   getWheelSpeeds(0, 50, 50, speedVector);
   setWheelSpeed(speedVector);
   // this should spin the car while moving x and y - direction
   getWheelSpeeds(40, 40, 40, speedVector);
   setWheelSpeed(speedVector);
   
   */


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



