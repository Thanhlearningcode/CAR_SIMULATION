# CAR_SIMULATION

## Overview

**CAR_SIMULATION** is a project that simulates a basic car control system using the PIC16F877A microcontroller. This project includes functionalities such as controlling the car's forward and reverse movements, managing speed, and utilizing various sensors and displays to provide feedback and control the car's behavior.

## Features

- **Forward/Reverse Control**: Ability to control the car's movement direction using digital inputs.
- **Speed Management**: Adjust the car's speed using PWM signals based on throttle and brake inputs.
- **Distance Measurement**: Use an ultrasonic sensor to measure distance and provide warnings.
- **LCD Display**: Display status messages, speed, and other information on an LCD.
- **Buzzer Alerts**: Use a buzzer to provide auditory feedback based on distance measurements.

## Components

- **Microcontroller**: PIC16F877A
- **PWM Drivers**: For motor control
- **Ultrasonic Sensor**: For distance measurement
- **LCD**: For displaying status and information
- **Buzzer**: For auditory feedback
- **Buttons/Switches**: For user input to control car movement and speed

## Hardware Setup

1. **Microcontroller**: Connect the PIC16F877A to your development board.
2. **PWM Pins**: Connect to the motors for controlling speed.
3. **Ultrasonic Sensor**: Connect the TRIG and ECHO pins to the designated GPIO pins.
4. **LCD**: Connect the LCD to the I2C or appropriate communication pins.
5. **Buzzer**: Connect to a digital output pin.
6. **Buttons/Switches**: Connect to digital input pins for user control.

## Software Setup

1. **IDE**: Use MPLAB X IDE or another compatible IDE for PIC16F877A programming.
2. **Compiler**: MPLAB XC8 or another compatible C compiler.
3. **Libraries**: Ensure that you have the necessary libraries for PWM, ADC, and LCD operations.

## Code Explanation

### Initialization

- **ADC Setup**: Configures ADC channels for reading sensor values.
- **PWM Setup**: Configures PWM outputs for motor speed control.
- **Timer Setup**: Configures timers for controlling servo motors and other timed operations.
- **Interrupts**: Sets up interrupts for external events and timer-based functions.

### Main Loop

1. **Speed and Direction Control**: Monitors button inputs to determine the car's movement direction and speed.
2. **Distance Measurement**: Uses the ultrasonic sensor to measure distance and provide warnings through the buzzer.
3. **LCD Updates**: Displays current status, speed, and messages on the LCD.
4. **PWM Adjustments**: Adjusts motor speeds based on throttle and brake inputs.

## Usage

1. **Compile the Code**: Use MPLAB X IDE to compile the provided C code.
2. **Upload to Microcontroller**: Flash the compiled code to the PIC16F877A microcontroller using a compatible programmer.
3. **Test and Operate**: Connect all hardware components as described in the Hardware Setup section and power up the system. Use the buttons to control the car's movement and monitor the LCD and buzzer for feedback.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgements

- [Microchip PIC16F877A Datasheet](https://www.microchip.com/wwwproducts/en/PIC16F877A)
- [MPLAB X IDE](https://www.microchip.com/mplab/mplab-x-ide)
- [MPLAB XC8 Compiler](https://www.microchip.com/mplab/compilers)

---

Feel free to adapt and expand this README as necessary to fit your project requirements.
