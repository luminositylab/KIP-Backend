#include "BusManager.h"

BusManager::BusManager() {
    _busParser = ParseOption();
    // Motor definitions

    // Left drive motor
    _dev.motor0.setSpeedMode();
    _dev.motor0.setSpeed(20);
    _dev.motor0.enable();

    // Right drive motor
    _dev.motor1.setSpeedMode();
    _dev.motor1.invert();
    _dev.motor1.setSpeed(20);
    _dev.motor1.enable();


    _dev.motor2.setSpeedMode();
    _dev.motor2.setSpeed(0);
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
    switch (c)
    {
        case 'M':
            _parseState = MOTOR;
            return;
            break;
        case 'S':
            _parseState = SERVO;
        default:
            break;
    }

    switch (_parseState)
    {
        case MOTOR:
            // run motor select
            switch (c)
            {
                case 0:
                    _motorSelect = MOTOR0;
                    break;
                case 1:
                    _motorSelect = MOTOR1;
                    break;
                case 2:
                    _motorSelect = MOTOR2;
                    break;
                default:
                    _motorSelect = IDLE;
                    break;
            }
            return;
            break;
        case SERVO:
            // sun servo select
            break;
        case NONE:
            break;
        default:
            break;
    }

    if (_parseState == MOTOR && _motorSelect != IDLE) {
        // update motor state
    }
}

void BusManager::feedData(char c) {
    _busParser.addCharToPool(c);
}

void BusManager::update(unsigned long dt) {
    _dev.motor0.update(dt);
    // _dev.motor1.update(dt);
}
