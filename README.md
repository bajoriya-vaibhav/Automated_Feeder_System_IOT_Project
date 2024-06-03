# Arduino IoT Project

This Arduino project is designed to control two stepper motors based on input from capacitive touch sensors and potentiometers. The project includes an LCD display for real-time feedback and uses EEPROM to store motor step values for persistence across power cycles.

## Table of Contents
- [Features](#features)
- [Components](#components)
- [Wiring Diagram](#wiring-diagram)
- [Installation](#installation)
- [Usage](#usage)
- [Code Explanation](#code-explanation)
- [License](#license)

## Features
- **Capacitive Touch Sensors**: Use touch sensors to control motor operations.
- **Stepper Motors**: Precisely control the position of two stepper motors.
- **EEPROM Storage**: Persist motor step values across reboots.
- **LCD Display**: Show current speed and volume settings.
- **Potentiometers**: Adjust motor speed and volume.

## Components
- Arduino Uno
- 2 Stepper Motors
- 2 Capacitive Touch Sensors
- 2 Potentiometers
- LCD Display (I2C, 16x2)
- EEPROM (built into Arduino)
- Connecting wires and breadboard

## Wiring Diagram
Connect the components as follows:

- **Capacitive Touch Sensors**:
  - Sensor 1: Pin 6 (send), Pin 7 (receive)
  - Sensor 2: Pin 2 (send), Pin 3 (receive)
- **Stepper Motors**:
  - Motor A: Pin 4, Pin 5
  - Motor B: Pin 8, Pin 9
- **Potentiometers**:
  - Potentiometer 1: A2 (speed), A3 (volume)
  - Potentiometer 2: A0 (speed), A1 (volume)
- **LCD Display**: Connect to I2C pins (SDA, SCL)

## Installation
1. **Clone the Repository**:
   ```sh
   git clone <repository-url>
