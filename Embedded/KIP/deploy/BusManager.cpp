#include "BusManager.h"

#define SYNC_BYTE 255
#define TOP_SPEED 50
BusManager::BusManager() {
    // Motor definitions

    // Left drive motor
    _dev.motor0.setSpeedMode();
    _dev.motor0.setSpeed(0);
    _dev.motor0.enable();

    // Right drive motor
    _dev.motor1.setSpeedMode();
    _dev.motor1.invert();
    _dev.motor1.setSpeed(0);
    _dev.motor1.enable();


    _dev.motor2.setPositionMode();
    _dev.motor2.setSpeed(100);
    _dev.motor2.enable();


    // Servo definitions
    // _dev.servo0.write(90);
}


// Manages the state of the robot and hardware during teleoperation mode.
// State Order:
// Hardware Target (M, S)
// Target ID (0..2)
// Target Value
void BusManager::teleopHandler(char c) {
    
}

void BusManager::feedData(char c) {
    if (c == (char)SYNC_BYTE) {
        _busState = NEXT_DIR_0;
        std::cout << "got sync byte" << std::endl;
        return;
    }
    switch (_busState) {
        case NEXT_DIR_0:
            _robotState.setLeftDriveDirection((int)c);
            // std::cout << "ldir: " << (int) c << std::endl;
            _busState = NEXT_MOTOR_0;
        break;
        case NEXT_MOTOR_0:
            // std::cout << "setting left to: " << ((int) c) << " adjusted: " << ((int)c) - 127<< std::endl; 

            _robotState.setLeftDriveSpeed( ((int)c) );
            _busState = NEXT_DIR_1;
        break;
        case NEXT_DIR_1:
            _robotState.setRightDriveDirection((int)c);
            // std::cout << "rdir: " << (int) c << std::endl;
            _busState = NEXT_MOTOR_1;
        break;
        case NEXT_MOTOR_1:
            // std::cout << "setting right to: " << (int) c << std::endl;
            _robotState.setRightDriveSpeed( ((int)c) );
            _busState = NEXT_MOTOR_2;
        break;
        case NEXT_MOTOR_2:
            // std::cout << "setting arm to: " << (int) c << std::endl;
            _robotState.setServo((int) c);
            _robotState.setPreciseArmPosition((float)c / 100.f);
            _busState = NEXT_SERVO;
            // std::cout << "set arm" << std::endl;
        break;
        case NEXT_SERVO:
            // std::cout << "servo: " << c << std::endl;
            _robotState.setServo((int) c);
            _busState = IDLE;
        break;
        case IDLE:
            // std::cout << "IDLE" << std::endl;
        break;
        default:
        break;
    }
}
float speedConverter(int speed) {
    return float(speed);
}

void BusManager::update(unsigned long dt) {
    if(_dev.motor0.getSpeed() == 0) {
        _dev.motor0.disable();
    } else {
        _dev.motor0.enable();
    }
    if(_dev.motor1.getSpeed() == 0) {
        _dev.motor1.disable();
    } else {
        _dev.motor1.enable();
    }

    if(_dev.motor2.getPosition() < 0.1) {
        _dev.motor2.enableSleep();
    } else {
        _dev.motor2.disableSleep();
    }
    // std::cout << "left: " << (float)_robotState.getLeftDriveSpeed() << std::endl;
    // std::cout << "right: " << (float)_robotState.getRightDriveSpeed() << std::endl;
    _dev.motor0.setSpeed( (((float)_robotState.getLeftDriveSpeed()/100.f) * TOP_SPEED) );
    _dev.motor1.setSpeed( ((float)_robotState.getRightDriveSpeed()/100.f) * TOP_SPEED );
    _dev.motor2.setPosition( _robotState.getPreciseArmPosition() );
    
    _dev.motor0.update(dt);
    _dev.motor1.update(dt);
    _dev.motor2.update(dt);
    

}
