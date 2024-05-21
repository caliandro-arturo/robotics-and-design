# Robotics and Design

Firmware for the robot that has been designed as a project for the
Robotics and Design course at Politecnico di Milano during the A.Y.
2023/2024.

## Board

Arduino Mega 2560, but it should work on any AVR architecture that can host enough pins to be populated by the components described in
`src/pins.hpp`.

## Used libraries

- [AceRoutine 1.5.1](https://github.com/bxparks/AceRoutine)
- [DFRobotDFPlayerMini 1.0.5](https://github.com/DFRobot/DFRobotDFPlayerMini)
- [Low-Power 1.81](https://github.com/rocketscream/Low-Power)
- [MATRIX7219 0.1.2](https://github.com/RobTillaart/MATRIX7219)
- [Servo 1.2.1](https://github.com/arduino-libraries/Servo)
- [TM1637 1.2.0](https://github.com/avishorp/TM1637)

## Project layout

- Any servo motor component is implemented as a class in the `src` folder;
- Power management, servo motor parameters and the pin configuration are in
respective sources and/or headers in the `src` folder;
- The other components, as well as the program logic, are directly
implemented in the `robotics-and-design.ino` source file.

## Authors

[Arturo Caliandro](https://github.com/caliandro-arturo) \
[Valentina Scorza](https://github.com/valentinascorza)