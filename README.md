# Universal DIY Remotecontroller

For my RC and robotics projects, I needed a better way to remote control my devices. 
So this is my attempt on building a handheld remote control, which I can use on any of 
my current and future projects.

![CAD design screenshot](https://githubcom/adrianfrisch/rc_controller/images/cad_design.png)
## Goals
The new controller should have two operation modes. Since I used the App "Bluetooth RC Controller" 
in the past for a project, It should have a bluetooth operation mode which operates similar and can be used 
instead of the mobile phone with app.
For other projects the transmission should be in the 2.4 GHz band to achieve a higher range.
The display should be readable also outdoors in the sun so I decided to go with a simple LCD 
module instead of a more fancy TFT.

## Features
- Bluetooth mode
- 2.4 GHz mode
- Push buttons for mode selection. After mode has been selected, can be used as switches to trigger actions
- After operation mode (BT / 2.4GHz) has been selected, a target device can be selected
- list of potential target devices will be configured in firmware
- two joystick knobs
- A display to show information
- power switch

## Benefits
- Just one remote necessary
- Reasonable price (I payed ~70€ for the parts)
- Very flexible
- Can display useful information or telemetry data on remote

## Specifications
This list contains all of the used components to put everything together
### Hardware / Bill of Materials
*Electronics*
- 1x Arduino Mega Microcontroller
- 1x HD44780 1602 LCD Module
- 1x NRF24L01 2.4GHz Transceiver
- 1x HC05 Bluetooth Transceiver
- 1x Power Switch
- 4x Push Buttons
- 2x 4-Axis joystick potentiometer
- 1x 2s 7.4V  900mAh Lipo battery
*3D printed parts*
- 1x lower body 
- 1x upper body
- 1x left panel
- 1x right panel
- 1x handle
- 1x display case front
- 1x display case backplate
- 1x battery cover plate
- 2x display connector
*Screws, nuts etc.*
- 10x M2 Screws 6mm
- ?x 2x8mm screws
- ?x 3x8mm screws
- hot glue
- CA glue
- heat shrink tube
## Test results
Work in progress. Not tested yet :)
## Identified problems
Work in progress. Not tested yet :)
## Future ideas
- The backplate of the LCD display mount needs to be glued at the moment. Add proper lid with screws.
- Calibration on power up as a "third mode selection" to allow recalib and eliminate need for 
  hardcoding in the firmware
- Save telemetry data to SD for latter analysis
- Build a corresponding universal receiver which can be used as a "plug in replacement" for a 
  standard RC-Receiver based on "16-channel PWM/Servo Shield -> NRF24L01 -> Arduino Nano"
  
