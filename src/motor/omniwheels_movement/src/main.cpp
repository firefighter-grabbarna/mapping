// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!
// Det var en gång, och den var grusad.

#include "main.h"

void setup()
{
   // 115200
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
param - omega_LUL is the angular speed
param - forwardSpeed is the speed in forwardSpeed-direction
param - sidewaysSpeed is the speed in sidewaysSpeed-direction

NOTE: since the matrix is of dimension 4 an arbitrary choice of u1,u2,u3,u4 can cause skidding
      (it is belived to be what it means)
      These vidoes are of concern:
      How the speed is calculated:
https://www.youtube.com/watch?v=NcOT9hOsceE

      This talks about how to limit of our speed needs to be taken into account:
      (one could just do low enought speed and this would probably not be a problem)
https://www.youtube.com/watch?v=B1K-ti5Lqjc

*/

/*
   Calculates the speed all wheels need for the vehicle to drive
   in a specified direction.
    - forwardSpeed positive forward
    - sidewaysSpeed positive left
    - rotationspeed positive counter-clockwise
*/
void getWheelSpeeds(int forwardSpeed, int sidewaysSpeed, int rotationSpeed, int (&speedVector)[4])
{
   speedVector[LEFT_FRONT_WHEEL - 1] = min(255, (forwardSpeed + rotationSpeed + sidewaysSpeed));  // 1 in coppa // left front
   speedVector[LEFT_BACK_WHEEL - 1] = min(255, (forwardSpeed - rotationSpeed + sidewaysSpeed));   // 2 in coppa // left back
   speedVector[RIGHT_BACK_WHEEL - 1] = min(255, (forwardSpeed + rotationSpeed - sidewaysSpeed));  // 3 in coppa // right back  ?
   speedVector[RIGHT_FRONT_WHEEL - 1] = min(255, (forwardSpeed - rotationSpeed - sidewaysSpeed)); // 4 in coppa // right front ?
}

/*
   Given a speed and a wheel
   sets the correct direction for the wheel

   A negative speed means backwards
   A positive speed means fortwads
*/
void setSpeedAndDirection(int speed, AF_DCMotor &wheel)
{
   wheel.setSpeed(min(255, abs((int)speed)));
   if (speed >= SPEED_THRESHOLD)
   {
      wheel.run(FORWARD);
   }
   else if (speed <= SPEED_THRESHOLD)
   {
      wheel.run(BACKWARD);
   }
}

/*
   Sets the speed of each wheel to the elements of speed_vect
*/
void setWheelSpeed(int (&speed_vect)[4])
{
   setSpeedAndDirection(speed_vect[LEFT_FRONT_WHEEL - 1], LeftFrontWheel);
   setSpeedAndDirection(speed_vect[RIGHT_BACK_WHEEL - 1], RightBackWheel);
   setSpeedAndDirection(speed_vect[RIGHT_FRONT_WHEEL - 1], RightFrontWheel);
   setSpeedAndDirection(speed_vect[LEFT_BACK_WHEEL - 1], LeftBackWheel);
}

// Doesn't work
void spinDegree(int degree, int (&speedVector)[4])
{
   const int timeTwoPiRotation = 3150;
   if (degree > 0)
   {
      getWheelSpeeds(0, 0, 150, speedVector);
   }
   else
   {
      getWheelSpeeds(0, 0, -150, speedVector);
   }
   setWheelSpeed(speedVector);
   delay(timeTwoPiRotation);
   motorStop();
   delay(3200);
}

/*
   Calculates the speed of each wheel and then runs.
    - forwardSpeed positive forward
    - sidewaysSpeed positive left
    - rotationspeed positive counter-clockwise
*/
void runWheels(int forwardSpeed, int sidewaysSpeed, int rotationSpeed){
   int speedVector[4];
   getWheelSpeeds(forwardSpeed, sidewaysSpeed, rotationSpeed, speedVector);
   setWheelSpeed(speedVector);
}

// Waits for input from serial and then updates response[] with said input
void listen(int (&response)[3]){
   
   while(!(Serial.available())){
      ; // Busy wait mycket effektivt
   }

   String input = Serial.readStringUntil('\n');
   char myArray[input.length() + 1];        //as 1 char space for null is also required
   strcpy(myArray, input.c_str());    
  
   char * token = strtok(myArray, " ");   // Extract the first token

   // loop through the string to extract all other tokens
   for(int i = 0; token != NULL; i++){
      response[i] = atoi(token);
      token = strtok(NULL, " ");
   }

   Serial.println("speed is: ");
   Serial.println(response[0]);
   Serial.println(response[1]);
   Serial.println(response[2]);

   // this should move the car in forwardSpeed-direction
   
}



void loop()
{
   // response contains:
   // forwardSpeed, sidewaysSpeed, rotationSpeed
   int response[3];
   listen(response);
   runWheels(response[0], response[1], response[2]);
   //runWheels(100, 100, 100);
   //delay(5000);
   //runWheels(-100, -100, -100);
   //delay(5000);
   
   //runWheels(0,0,255);
   //delay(5000);
   //runWheels(255,0,0);
   //delay(5000);
   //runWheels(0,255,0);
   //delay(5000);
   //runWheels(120,120,0);
   //delay(5000);

}
void motorStop()
{
   LeftFrontWheel.run(RELEASE);
   LeftBackWheel.run(RELEASE);

   RightFrontWheel.run(RELEASE);
   RightBackWheel.run(RELEASE);
}
