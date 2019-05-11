/*
  BusManager.h - A state machine based parser for interperting commands over I2C.
*/
#include <DevBoardLayout.h>
#include "ParseOption.h"
#ifndef __BUSMANAGER_H_
#define __BUSMANAGER_H_

enum OperatorState {
  TELEOP,
  AUTO
};

enum TeleopParseState {
  MOTOR,
  SERVO,
  NONE
};

enum MotorSelect {
  MOTOR0,
  MOTOR1,
  MOTOR2,
  IDLE
};

class BusManager {
    public:
      BusManager();
      void update(unsigned long dt);  // Updates the physical hardware state based bus state
      void feedData(char c);          // Pass chars from bus line -- this function passes to the state machine
      DevBoardLayout _dev = DevBoardLayout();
      ParseOption _busParser;
    private:
      void teleopHandler(char c);

      
      OperatorState _opState = TELEOP;
      TeleopParseState _parseState = NONE;
      MotorSelect _motorSelect = IDLE;

};
#endif