#include <ArduinoSTL.h>
#include <Servo.h>
#ifndef __ROBOTSTATE_H_
#define __ROBOTSTATE_H_
class RobotState {
    public:
        RobotState();
        void begin();
        float getPreciseArmPosition();
        void setPreciseArmPosition(float position);
        void setLeftDriveSpeed(int speed);
        void setRightDriveSpeed(int speed);
        int getLeftDriveSpeed(); // returns speed in RPM
        int getRightDriveSpeed(); // returns speed in RPM
        void setServo(int pos);
        int getServo();
        void setLeftDriveDirection(int dir);
        void setRightDriveDirection(int dir);


        void setArmPosition(int position); // sets the arm position in rotations
        int getArmPosition(); // returns the position of the arm in roations
    private:
        Servo _mainServo;
        int _servoPos; // servo position
        int _leftSpeed; // in RPM
        int _rightSpeed; // in RPM
        int _armPosition; // in rotations
        bool _leftPositive;
        bool _rightPositive;
        float _preciseArmPosition; 

};

#endif