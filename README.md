Arduino firmware for a "Cone to Hank" winding machine
=======

- **Hardware Description**
  - MCU: Arduino Leonardo (atmega32u4)
  - Display: 1.3" 128x64 I2C SH1106 OLED Display
  - Navigation: Joystick module
  - Motor controller: L298N
  - Turn sensor: Optical detector 
  - Start/Pause/Stop button with embedded LED

![Schéma](https://github.com/jpcornil-git/Cone2Hank/assets/40644331/d0791cf7-6372-4a6b-af8f-459a7f5bfe44)

- **Firmware features**
  - Navigation menu
    - Joystick control (up/down/left/right/select)
    - Configure number of turns and motor speed
    - Manual motor control
    - Save current settings as default in eeprom
  - Start/Stop/Pause button/led
  - Control motor (direction and speed/pwm)
  - Counts turns based on optical encoder inputs
  - Watchdog reset & recovery
  - Splash screen & screensaver

... only ~100 bytes left in SRAM between heap/stack ...

- **Development environment**
  - Microsoft's Visual Studio Code with PlatformIO IDE extension
