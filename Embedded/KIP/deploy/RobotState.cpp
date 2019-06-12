#include "RobotState.h"

RobotState::RobotState() {
    _leftSpeed = 0;
    _rightSpeed = 0;
    _armPosition = 0;
    _rightPositive = true;
    _leftPositive = true;
}

void RobotState::setLeftDriveSpeed(int speed) {
    _leftSpeed = speed;
}

void RobotState::setRightDriveSpeed(int speed) {
    _rightSpeed = speed;
}

int RobotState::getLeftDriveSpeed() {
    if (_leftPositive) {
        return _leftSpeed;
    } else {
        return -1 * _leftSpeed;
    }
}

int RobotState::getRightDriveSpeed() {
    if (_rightPositive) {
        return _rightSpeed;
    } else {
        return -1 * _rightSpeed;
    }
}

void RobotState::setArmPosition(int position) {
    _armPosition = position;
}
void RobotState::setPreciseArmPosition(float position) {
    _preciseArmPosition = position;
}
int RobotState::getArmPosition() {
    return _armPosition;
}
float RobotState::getPreciseArmPosition() {
    return _preciseArmPosition;
}

void RobotState::setLeftDriveDirection(int dir) {
    _leftPositive = dir > 0;
}
void RobotState::setRightDriveDirection(int dir){
    _rightPositive = dir > 0;
}
