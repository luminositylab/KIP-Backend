#include <ArduinoSTL.h>
#ifndef __ROBOTSTATE_H_
#define __ROBOTSTATE_H_
class RobotState {
    public:
        RobotState();
        void setLeftDriveSpeed(char speed);
        void setRightDriveSpeed(char speed);
        char getLeftDriveSpeed(); // returns speed in RPM
        char getRightDriveSpeed(); // returns speed in RPM

        void setArmPosition(char position); // sets the arm position in rotations
        char getArmPosition(); // returns the position of the arm in roations
    private:
        char _leftSpeed; // in RPM
        char _rightSpeed; // in RPM
        char _armPosition; // in rotations

};

#endif