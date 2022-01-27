#include <Arduino.h>
#include <Servo.h> 

static int X_SERVO_PIN = 9;
static int X_FLAME_PIN = A0;
static int Y_SERVO_PIN = 10;
static int Y_FLAME_PIN = A1;
Servo xAxisServo;
Servo yAxisServo;
void setup() {
  // put your setup code here, to run once:

  // initialize serial communication @ 9600 baud:
  Serial.begin(9600);  
  pinMode(X_SERVO_PIN, OUTPUT);
  pinMode(X_FLAME_PIN, INPUT);
  
  xAxisServo.attach(X_SERVO_PIN);

}

  /*
  *
  * Returns the degree which gives the largest reading on flamePin 
  * from one cycle of 0-180 degrees
  * 
  */  

int getSmallestReading(Servo& servo, int flamePin){
  servo.write(0);
    
  delay(1000);
  int maxDegree = 180;
  int nearestDegree = 0;
  int smallestReading = 90000;
  for (int degree = 0; degree < maxDegree; degree++){
    int reading = analogRead(flamePin);
    Serial.println(reading);
    servo.write(degree);
    delay(50);
    if(reading < smallestReading){
      smallestReading = reading;
      nearestDegree = degree;
    }     
  }
  return nearestDegree;
} 

/*
  Moves the servo to the degree which returns the smallest
  reading from the flame sensor 

  Smallest reading is the one which is closest to the flame
*/

void moveToLargestReading(Servo& servo, int flamePin){
  delay(100);
  int degree = getSmallestReading(servo, flamePin);
  xAxisServo.write(degree);
  Serial.println(degree);
  delay(2000);

}


void loop() {
  // put your main code here, to run repeatedly:
  /*
   * The idea is to scan one axis with the flame sensor, 
   * get the servo degree for the largest value,
   * then return the servo to the greatest degree.
   */

  moveToLargestReading(xAxisServo, X_FLAME_PIN);
  moveToLargestReading(yAxisServo, Y_FLAME_PIN);
}

