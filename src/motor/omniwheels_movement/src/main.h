#ifndef MAIN_H
#define MAIN_H

#include <AFMotor.h>
#include <Arduino.h>

enum directions{
    MOVE_FORWARD, MOVE_BACKWARD, MOVE_STOP
};

void setup();

int convertSpeed(int speed);
void setSpeed(int speed);

directions getDir(int pwmValue);

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

#endif