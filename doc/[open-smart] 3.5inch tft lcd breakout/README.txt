First you should know the breakout board is 3.3V logic level and it can not connect to 5V Arduino board directly.
You have to choice:
1: Do not plug USB cable and supply external 3.3V power for the 5V pin on the Arduino board, so that it works at 3.3V logic level.
2: Use some 3.3V MCU development board, such as OPEN-SMART UNO R3 that has a 5V/3.3V switch to select 5V or 3.3V logic level.

//********WIRING GUIDE**********//
// If you use OPEN-SMART TFT breakout board                 //
// Reconmmend you to add 5V-3.3V level converting circuit.
// Of course you can use OPEN-SMART UNO Red version with 5V/3.3V power switch,https://www.aliexpress.com/store/product/One-Set-UNO-R3-CH340G-ATMEGA328P-Development-Board-with-USB-Cable-for-Arduino-UNO-R3-Compatible/1199788_32653902890.html?spm=2114.12010615.8148356.57.2f68719990gKa6
// you just need switch to 3.3V. 
// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as follow.
//----------------------------------------|
// TFT Breakout  -- OPEN-SMART UNO Red
// GND              -- GND
// 3V3              -- 3.3V
// CS               -- A3
// RS               -- A2
// WR               -- A1
// RD               -- A0
// RST              -- RESET
// LED              -- GND
// DB0              -- 8
// DB1              -- 9
// DB2              -- 10
// DB3              -- 11
// DB4              -- 4
// DB5              -- 13
// DB6              -- 6
// DB7              -- 7
//***************************************************

Delete all the TFT libraries you have updated, and then follow me.
Download [OPEN-SMART] TFT LCD 3.0inch R61509V.rar
from https://drive.google.com/drive/folders/1RD0MlwHU44_1awyqCGptSnMxpQIjxt85?usp=sharing
The library is compatible with Arduino UNO and MEGA2560.
/***********************************************************************************************************/
!!!NOTE:
DO NOT! DO NOT! DO NOT update directly from the Arduino IDE.
You should know that the library you update directly from the Arduino IDE is not provided by us.
Maybe it is written by somebody else.
And first delete any MCUFRIEND_kbv named files in the Arduino IDE libraries directory.
Please follow me: and do not update from Arduino IDE
Please download the new library from the google driver: https://drive.google.com/drive/folders/1RD0MlwHU44_1awyqCGptSnMxpQIjxt85?usp=sharing
All the four libraries in the folder of "Arduino library" must be put in the derectory of ../Arduino-1.xx/libraries, and then restart the IDE. 
We have test the four examples in the derectory of D:\arduino-1.6.5-r2\libraries\OPENSMART_TFT\examples\3.5inch RM68140.
/*************************************************************************************************************/