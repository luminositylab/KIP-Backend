/*
  BusManager.h - A state machine based parser for interperting commands over I2C.
*/

#include <ArduinoSTL.h>
#include <DevBoardLayout.h>
#include "ParseOption.h"
#include "RobotState.h"
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

enum BusState {
  NEXT_MOTOR_0,
  NEXT_MOTOR_1,
  NEXT_MOTOR_2,
  IDLE
};
class BusManager {
    public:
      BusManager();
      RobotState _robotState = RobotState();
      void update(unsigned long dt);  // Updates the physical hardware state based bus state
      void feedData(char c);          // Pass chars from bus line -- this function passes to the state machine
      DevBoardLayout _dev = DevBoardLayout();
    private:
      void teleopHandler(char c);
      BusState _busState = IDLE;
      OperatorState _opState = TELEOP;
      TeleopParseState _parseState = NONE;

};
#endif