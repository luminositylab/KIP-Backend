#include "BusManager.h"
#define SYNC_BYTE 255

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
    _dev.motor2.setSpeed(200);
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
    if (c == (char) SYNC_BYTE) {
        _busState = NEXT_MOTOR_0;
        std::cout << "got sync byte" << std::endl;
        return;
    }
    switch (_busState) {
        case NEXT_MOTOR_0:
            // std::cout << "setting left to: " << (int) c << std::endl; 
            _robotState.setLeftDriveSpeed(c);
            _busState = NEXT_MOTOR_1;
        break;
        case NEXT_MOTOR_1:
            // std::cout << "setting right to: " << (int) c << std::endl;
            _robotState.setRightDriveSpeed(c);
            _busState = NEXT_MOTOR_2;
        break;
        case NEXT_MOTOR_2:
            // std::cout << "setting arm to: " << (int) c << std::endl;
            float pos = (float)c / 100.f;
            _robotState.setPreciseArmPosition(pos);
            _busState = IDLE;
        break;
        case IDLE:
        break;
        default:
        break;
    }
}
float speedConverter(int speed) {
    return float(speed) * (200.f/127.f);
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
    _dev.motor0.setSpeed( speedConverter((int)_robotState.getLeftDriveSpeed()) );
    _dev.motor1.setSpeed( speedConverter((int)_robotState.getRightDriveSpeed()) );
    _dev.motor2.setPosition( _robotState.getPreciseArmPosition() );

    _dev.motor0.update(dt);
    _dev.motor1.update(dt);
    _dev.motor2.update(dt);
}
