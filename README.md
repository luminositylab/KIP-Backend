# KIP-Backend

v2.0.0-Beta // Written for XK-02

Official website: [kipkit.io](https://kipkit.io/)

Writer and maintainer: G M Rowland // [gmichaelrowland.com](http://gmichaelrowland.com) // g@gmichaelrowland.com

## Overview

In terms of physical hardware stack of KIP, it uses a raspberry pi as a linux server for running python robot code and a custom PCB as a stepper driver, PWM controller, and general DIO and AIO. The PCB is run by an [ATMEGA2560](https://microchip.com/wwwproducts/en/ATmega2560) which drives standard [A4988](https://pololu.com/product/1182) stepper driver.

The software is split into the server code that runs on the raspberry pi and the embedded code that runs on the ATmega2560. The server code is written to be able to handle both http route connections as well as standard socketio connections. The server stack is Flask for networking with multiple threads to handle robot execution code and robot state as well as I2C bus communication. The embedded code is Arduino C++ which is written to be a non-blocking stepper driver, I2C slave, and general AIO and DIO state manager.


## Arduino C++ Dependencies

- [ArduinoSTL](https://github.com/mike-matera/ArduinoSTL)
- [StepperController](https://github.com/GowanR/StepperController)
