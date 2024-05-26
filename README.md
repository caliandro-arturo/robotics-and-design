# Robotics and Design

Firmware for Romeow, the robot that has been designed as a project
for the Robotics and Design course at Politecnico di Milano during
the A.Y. 2023/2024.

## Requirements

### Software

There are three options here:
1. [arduino-cli](https://arduino.github.io/arduino-cli/latest/installation);
2. [VS Code](https://code.visualstudio.com)*;
3. [Arduino IDE](https://www.arduino.cc/en/software).

\* for VS Codium users: \
The Arduino extension and its dependencies (namely C/C++ and Serial Monitor)
are released by Microsoft, so you have to [download and install them manually](https://code.visualstudio.com/docs/editor/extension-marketplace#_install-from-a-vsix).


### Libraries

The following libraries are already included into the repo, in the `libs` folder:

- [AceRoutine 1.5.1](https://github.com/bxparks/AceRoutine)
- [DFRobotDFPlayerMini 1.0.5](https://github.com/DFRobot/DFRobotDFPlayerMini)
- [Low-Power 1.81](https://github.com/rocketscream/Low-Power)
- [MATRIX7219 0.1.2](https://github.com/RobTillaart/MATRIX7219)
- [Servo 1.2.1](https://github.com/arduino-libraries/Servo)
- [TM1637 1.2.0](https://github.com/avishorp/TM1637)

### Board

Arduino Mega 2560, but it should work on any AVR architecture that
can host enough pins to be populated by the components described in
`src/pins.hpp` and is supported by the libraries listed above.

## How to install

This depends on which option you selected [here](#software):

### arduino-cli

Plug your board and simply run `build.sh`.

### VS Code

- Install the [Arduino extension](https://github.com/microsoft/vscode-arduino);
- Run `Arduino: Initialize` from the command palette;
- Select `Arduino Mega or Mega 2560` (or your board);
- Open `.vscode/arduino.json` and append `"output": "build"` to the object;
- Open `.vscode/c_cpp_properties.json` and
    - Change the configuration name from `Arduino` to something else, like `Arduiyes`;
    - Append to the `compilerArgs` array the argument `"-mmcu=atmega2560"` (or the name of your mcu);
    - Append to the `includePath` array the argument `"${workspaceFolder}/libs/**"`

then select `Arduiyes` as the configuration for the project (at the bottom right
of the VS Code window).

### Arduino IDE

I did not figure out how to include source files from non-standard folders, so
just install the [required libraries](#libraries).

## Project layout

- Any servo motor component is implemented as a class in the `src` folder;
- Power management, servo motor parameters and the pin configuration are in
respective sources and/or headers in the `src` folder;
- The other components, as well as the program logic, are directly
implemented in the `robotics-and-design.ino` source file.

## Authors

[Arturo Caliandro](https://github.com/caliandro-arturo) \
[Valentina Scorza](https://github.com/valentinascorza)
