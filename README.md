# STRemoteMusicPlayer

## Overview
The project of simple .wav player based on STM32f4-DISCOVERY board. It plays .wav files from SD card.

## Description
The idea to play .wav files was implement the two-way cyclic list. It contains information about files. We use SPI for sending data from SD card to STM32F4. It uses DMA module for smoother playback of music. To control which song we are playing we use App for IPhone, you can pause and resume songs, also it's posible to control volume. What is more, the simple error handling was implemented. When you spot blinking:
* green LED - cable fault, SD Card isn't inserted into SD Card Module or SD Card isn't formatted to FAT32,
* orange LED - SD Card has been removed during listening music,
* red LED - SD Card doesn't include .wav files.

## Tools
- CooCox CoIDE, Version: 1.7.8

## How to run
To run the project you should have hardware:
- STM32f4-DISCOVERY board,
- SD Card Module and SD Card formatted to FAT32,
- BLE Module for STM32 board,
- Headphone or loudspeaker with male jack connector,
- IPhone with BLE

How to use?

1. Connect STM32F4-DISCOVERY board with SD Card Module in this way:
  * STM32 <---> SD Card Module
  * GND  <---> GND
  * 3V   <---> 3V3
  * PB11 <---> CS
  * PB15 <---> MOSI
  * PB13 <---> SCK
  * PB14 <---> MISO
  * GND  <---> GND
 
 2. Connect STM32F4-DISCOVERY board with SD Card Module in this way:
  * STM32 <---> BLE Module
  * GND  <---> GND
  * 3V   <---> 3V3
  * PA09 <---> Rx
  * PA10 <---> Tx
 
3. Plug your SD Card with .wav files into the module.

4. Build this project with CooCox CoIDE and Download Code to Flash.

5. Connect IPhone with card via BLE

6. When you notice the fault alarmed by blinking LEDs, you should fix it and press RESET button.

## How to compile
The only step is download the project and compile it with CooCox CoIDE.

## Attributions
- http://elm-chan.org/fsw/ff/00index_e.html

## License
MIT

## Credits
* Daniel Lachowicz
* Piotr Mielcarzewicz

The project was conducted during the Microprocessor Lab course held by the Institute of Control and Information Engineering, Poznan University of Technology.

Supervisor: Tomasz Mańkowski
