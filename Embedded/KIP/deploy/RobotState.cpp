#include "RobotState.h"

RobotState::RobotState() {
    _leftSpeed = 0;
    _rightSpeed = 0;
    _armPosition = 0;
}

void RobotState::setLeftDriveSpeed(char speed) {
    _leftSpeed = speed;
}

void RobotState::setRightDriveSpeed(char speed) {
    _rightSpeed = speed;
}

char RobotState::getLeftDriveSpeed() {
    return _leftSpeed;
}

char RobotState::getRightDriveSpeed() {
    return _rightSpeed;
}

void RobotState::setArmPosition(char position) {
    _armPosition = position;
}

char RobotState::getArmPosition() {
    return _armPosition;
}