#include <AFMotor.h>
#include <Arduino.h>

void setup();

int convertSpeed(int speed);
void setSpeed(int speed);

void moveForward();
void moveBackward();
void moveSidewaysRight();
void moveSidewaysLeft();

void rotateLeft();
void rotateRight();

void moveRightForward();
void moveRightBackward();
void moveLeftForward();
void moveLeftBackward();

void motorStop();

