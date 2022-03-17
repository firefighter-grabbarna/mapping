#include <Arduino.h>
#include <Servo.h> 

static int Y_SERVO_REST_POS = 60;
static int Y_SERVO_VERTICAL_OFFSET = -45;
static int X_SERVO_PIN = 3;
static int X_FLAME_PIN = A0;
static int Y_SERVO_PIN = 5;
static int Y_FLAME_PIN = A0;
Servo xAxisServo;
Servo yAxisServo;
void setup() {
  // put your setup code here, to run once:

  // initialize serial communication @ 9600 baud:
  Serial.begin(9600);  
  pinMode(X_SERVO_PIN, OUTPUT);
  pinMode(X_FLAME_PIN, INPUT);
  pinMode(Y_SERVO_PIN, OUTPUT);
  pinMode(Y_FLAME_PIN, INPUT);
  
  xAxisServo.attach(X_SERVO_PIN);
  yAxisServo.attach(Y_SERVO_PIN);

}

  /*
  *
  * Returns the degree which gives the largest reading on flamePin 
  * from one cycle of 0-180 degrees
  * 
  */  


int getSmallestReading(Servo& servo, int flamePin, int minDegree, int maxDegree){
  servo.write(minDegree);
    
  delay(400);
  int nearestDegree = 0;
  int smallestReading = 90000;
  for (int degree = minDegree; degree < maxDegree; degree++){
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



int getSmallestReading(Servo& servo, int flamePin){
  getSmallestReading(servo, flamePin, 0, 180);
} 

/*
  Moves the servo to the degree which returns the smallest
  reading from the flame sensor 

  Smallest reading is the one which is closest to the flame
*/

void moveToLargestReading(Servo& servo, int flamePin, int minDegree, int maxDegree) {
  delay(100);
  int degree = getSmallestReading(servo, flamePin, minDegree, maxDegree);
  servo.write(degree);
  //Serial.println(degree);
  delay(400);

}

void moveToLargestReading(Servo& servo, int flamePin){
  moveToLargestReading(servo, flamePin, 0, 180);

}


void loop() {
  // put your main code here, to run repeatedly:
  /*
   * The idea is to scan one axis with the flame sensor, 
   * get the servo degree for the largest value,
   * then return the servo to the greatest degree.
   */
   if (Serial.available()) {
    String s = Serial.readStringUntil('\n');
    yAxisServo.write(Y_SERVO_REST_POS);
    moveToLargestReading(xAxisServo, X_FLAME_PIN);
    xAxisServo.write(xAxisServo.read()+Y_SERVO_VERTICAL_OFFSET);
    delay(300);
    moveToLargestReading(yAxisServo, Y_FLAME_PIN,40, 120);
    xAxisServo.write(xAxisServo.read()-Y_SERVO_VERTICAL_OFFSET);  
   }
  }