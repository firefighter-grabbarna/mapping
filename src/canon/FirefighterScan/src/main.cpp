#include <Arduino.h>
#include <Servo.h> 

#define Y_SERVO_REST_POS 110
#define Y_SERVO_VERTICAL_OFFSET -45

static int X_SERVO_PIN = 3;
static int X_FLAME_PIN = A0;
static int Y_SERVO_PIN = 5;
static int Y_FLAME_PIN = A0;
static int BUTTON_GROUND = 8;
static int BUTTON_PIN = 6;
static int TRANSISTOR_PIN = 4;
static short VALS[180];

Servo xAxisServo;
Servo yAxisServo;

void setup() {

  // initialize serial communication @ 9600 baud:
  Serial.begin(9600);  
  pinMode(X_SERVO_PIN, OUTPUT);
  pinMode(X_FLAME_PIN, INPUT);
  pinMode(Y_SERVO_PIN, OUTPUT);
  pinMode(Y_FLAME_PIN, INPUT);
  pinMode(BUTTON_GROUND, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(TRANSISTOR_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  digitalWrite(TRANSISTOR_PIN, LOW);
  digitalWrite(BUTTON_GROUND, LOW);
  digitalWrite(LED_BUILTIN, LOW);

  xAxisServo.attach(X_SERVO_PIN);
  yAxisServo.attach(Y_SERVO_PIN);
}

void read_along_axis(Servo& servo, int flame_pin, int startDegree, int maxDegree) {
  memset(VALS, 0, sizeof(VALS));
  int step_size = 1;
  servo.write(startDegree);
  delay(500);
  for (int i = startDegree; i < maxDegree; i+=step_size) {
    VALS[i] = analogRead(flame_pin);
    servo.write(i);
    delay(50);
  }
}

int choose_degree() {
  short min_value = 32000;
  for (int i = 0; i < 180; ++i) {
    if (VALS[i] == 0) {
      continue;
    }
    min_value = min(VALS[i], min_value);
  }
  Serial.println(min_value);
  int diff = 20;
  int best_index = 0;
  for (int i = 0; i < 180; ++i) {
    if (VALS[i] == 0) {
      continue;
    }
    if (VALS[i] - min_value < diff) {
      best_index = i;
    }
  }
  return best_index;
}

int choose_degree_y() {
  short min_value = 32000;
  for (int i = 0; i < 180; ++i) {
    if (VALS[i] == 0) {
      continue;
    }
    min_value = min(VALS[i], min_value);
  }
  Serial.println(min_value);
  int diff = 20;
  int best_index = 0;
  for (int i = 180-1; i >= 0; --i) {
    if (VALS[i] == 0) {
      continue;
    }
    if (VALS[i] - min_value < diff) {
      best_index = i;
    }
  }
  return best_index;
}

void loop() {

   if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command == "SCIP2.0") {
      Serial.println("CANNON");
      Serial.println("");
    } else if (command == "W") {
      digitalWrite(LED_BUILTIN, HIGH);
      while (digitalRead(BUTTON_PIN));
      while (!digitalRead(BUTTON_PIN));
      Serial.println("pressed");
      Serial.println("");
      digitalWrite(LED_BUILTIN, LOW);
    } else if (command == "S") {
      read_along_axis(xAxisServo, X_FLAME_PIN, 0, 180);
      int degree = choose_degree();
      xAxisServo.write(degree-9);
      read_along_axis(yAxisServo, Y_FLAME_PIN, 75, 105);
      degree = choose_degree_y();
      yAxisServo.write(degree+11);   
      delay(300);
      digitalWrite(TRANSISTOR_PIN, HIGH);
      delay(300);
      digitalWrite(TRANSISTOR_PIN, LOW);
    } else if (command == "A") {
      int degree = choose_degree();
      Serial.println(degree);
      Serial.println("");
    }
   }
  }