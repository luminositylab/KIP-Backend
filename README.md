# KIP-Backend

All of the code that runs on the raspberry pi and arduino on the robot.

TODO: 
- Servo arduino code
- Motion profiler buffer sending
- Rate limited serial calls (flask side)
- Absolute position motor control
- API documentation




 ___________                    ______________ <br>
|           |                  |              |<br>
|  Client   | <==== HTTP ====> | Flask Server |<br>
|___________|                  |______________|<br>
                                     ||<br>
                                     || Serial<br>
                                     ||<br>
                                     \/<br>
                                 __________<br>
                                |          |<br>
                                | Arduino  |<br>
                                |__________|<br>
