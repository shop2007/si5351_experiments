Band_Surfer_VFO_ESP32S3_03_16_24_V1_0.ino

In order to use this code you will need to do the following

The code is working with the ILI9341 display, however the  DOT_MATRIX_LCD sections are yet completed.

************************************************************************ 
You will need the following libraries installed on the Arduino IDE.
ezButton                        by ArduinogetStarted.com V1.04
U8g2 				by Oliver V2.34.22
LedController 			by Noa Sakurajin V2.02
Adafruit FRAM I2C 		V 2.0.3
Ucglib                          V1.5.2    by Oli Kraus           
Adafruit seesaw Library         V1.7.5     and all dependencies.

***********************************************************************
Also you will need to configure the Arduino IDE to take the ESP32S3.

Arduino IDE 
Top left and select "File"
Select  "Preferences"
Add the following URL to Additional Boards Manager URL's box.

https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json 

hit OK

In board manager select "ESP32S3 Dev Module"
This should now compile, let me know how you get on as you can help me iron out any bugs before.

**********************************************************************
In the user definable area, remove the // from macros to use the feature

 //#define MAX7219
#define SSD1306_DISPLAY_X
#define ILI9341_DISPLAY_X
//#define CLK1_ENABLE //WORK IN PRORESS!!!
//#define DOT_MATRIX_LCD //WORK IN PRORESS!!!
//#define CHANNEL_SET
#define VFO_LOCK
#define SI5351_X
//#define TIME_UTC << you will need to connect to wifi and set *wifi_SSID and *wifi_Password or it will hang
//#define FRAM_X   <<<you may have to scan the i2c to find the address and change and set FRAM_I2C_address

Note to use FRAM you will need to set bool reset = false; to true then upload sketch to arduino. THEM change back to false and upload to Arduion.
it should now be fully fuctional.

Let me knoq how you get on m0xfx.channel@gmail.com
