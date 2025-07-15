/* 
 * Band_Surfer_VFO_ESP32S3_03_16_24_V1_0.ino - VFO sketch with IF offsets and displays.
 * mode and TX sensing with mode frequency shifting options added, band counter fixed and encoder lag issue fixed. Added 11 meter channels, dot matrix LCD display and vfo lock                          
 * Added ILI9341
 * Copyright 2024 John Hawkes (M0XFX) <m0xfx.channel@gmail.com>
 *                       
 * 
 *
 * This sketch  is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 003 of the License, or
 * (at your option) any later version.
 *
 * Please respect any conditions set out in the libraries used in this sketch.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For futher information about this VFO build and code, look at Youtube channel https://www.youtube.com/@M0XFXUK
 * 
*/

#include <Wire.h>
#include <ezButton.h>

//********************************************************************************************************************************************************************************************|
//********************************************************************************************************************************************************************************************|
//****************************************************************************USER DEFINABLE SECTION******************************************************************************************v
/*
NOTE: The #define macros will switch in functions as required  
MAX7219 is the seven segment eight digit display
SSD1306_DISPLAY_X is for the 128x64 I2C OLED dispay 
CLK1_ENABLE is to use a fixed second frequency on CLK1. Note: CLK0 is defined by default.
DOT_MATRIX_LCD is for an I2C 20x4 dot matrix LCD display
CHANNEL_SET is to add channels, this will require and I2C Adafruit encoder board and a steped rotory encoder at 1 pulse per detent
VFO_LOCK add a VFO lock fuction. Press ond hold Speed Button for 2 second to lock and press once to unlock
Comment in the folowing macros to use the funtctions. Note this sketch is for the ESP32-S3 N8R8 Dev Board.
*/
//#define MAX7219
#define SSD1306_DISPLAY_X
#define ILI9341_DISPLAY_X
//#define CLK1_ENABLE //WORK IN PRORESS!!!
//#define DOT_MATRIX_LCD //WORK IN PRORESS!!!
//#define CHANNEL_SET
#define VFO_LOCK
#define SI5351_X
//#define TIME_UTC
//#define FRAM_X


uint8_t LED_brightness = 15; //set LED brightness range 1 - 15 for MAX7219 seven segment dispalys
//CLK0
long IF_freqHz = 10695000; //IF frequency in Hz, If the IF shift is not required set IF_freqHz to 0 and make sure up_down_conversion is set to 1 (The clock generator will then produce the dispaly frequency) //60000000

const uint8_t number_of_bands = 17;
unsigned long band_freqHz[number_of_bands]{27000000,1476000, 1800000, 3500000, 5351000, 7000000, 10100000, 14000000, 18068000, 21000000, 24890000, 27000000, 28000000, 50000000, 70000000, 88000000, 144000000};
static String band_name[number_of_bands] = {"","MW", "160M", "80M", "60M", "40M", "30M", "20M", "17M", "15M", "12M", "11M", "10M", "6M", "4M", "FM", "2M"}; //band name array
int up_down_conversion = 1; // 1 = down conversion, -1 = up conversion mixer
bool invert_encoder_direction = false; // change direction of rotary encoder counting true/false 
static float bar_meter_lev = 1.42; // signal meter level adjust max is around 3
long Cal_si5351 = 122360;//122360; // need to go through calibration routine to get this figure
const char FRAM_I2C_address = 0x57;
const char *wifi_SSID = "iPhone";//wifi SSID / Name
const char *wifi_Password = "123abctest";//wifi password
static bool modeDisplayenable = true; //Display mode selected
static bool modeShiftenable = true; //Enables mode frequency shifting
static bool m_modeShiftenable = false;
int AM_ShiftHz = 0; //Frequency shift in Hz for AM mode, can be positive or negative value
int FM_ShiftHz = 0; //Frequency shift in Hz for FM mode, can be positive or negative value
int USB_ShiftHz = 0; //Frequency shift in Hz for USB mode, can be positive or negative value
int LSB_ShiftHz = -3000; //Frequency shift in Hz for LSB mode, can be positive or negative value
int CW_ShiftHz = 0; //Frequency shift in Hz for CW mode, can be positive or negative value
static bool rxtxDisplayenable = true; //Displays RX/TX mode
static bool TX8shiftenable = false; //Enables mode TX shifting
static long TX8_ShiftHz = 0; //Frequency shift in Hz for TX mode, can be positive or negative value
static bool modeShift_TX_enable = false; //Enables mode frequency shifting in TX mode as well as RX 
bool sql_enable = true; // squlech control and squelch mute
bool reset = false;
//****************************************************************************USER DEFINABLE SECTION******************************************************************************************^
//********************************************************************************************************************************************************************************************|
//********************************************************************************************************************************************************************************************|

#ifdef FRAM_X
#include "FRAM_Backup.h"
#define FRAM_BACKUP_H
FRAM_MEM MB85RC256;
#endif

#ifdef TIME_UTC
#include "UTC_Time.h"
#define UTC_TIME_H
UTC_TIME UTC_TIMEX;
#endif

#define SIG_ADC_LEV 5
#define SQL_ADC_LEV 6
#define SQL_MUTE 41
#ifdef SI5351_X
#include "SI5351_Clock_Gen.h"
#define SI5351_CLK_GEN_H
SI5351_CLK_GEN CLK_SI5351;
#endif

#ifdef SSD1306_DISPLAY_X
#include "SSD1306_Display.h"
#define SSD1306_DISPLAY_H
SSD1306_DISPLAY SSD1306;
#endif

#define band_select_A   0
#define band_select_B   1
#define vfo_num         2 
#define tuning_select   3
#define speed_select    4
const uint8_t number_of_gen_set = 5;
int gen_set_mem_array[number_of_gen_set]{1,3,1,1,1};
int m_gen_set_mem_array[number_of_gen_set];
unsigned sql_millis;
//rxtx vfo veriables
int updateSysStack = 0;
long band_freqHz_A[number_of_bands];
long band_freqHz_B[number_of_bands];
long m_band_freqHz_A[number_of_bands];
long m_band_freqHz_B[number_of_bands];
int m_vfo_num;
#define vfo_A 1 
#define vfo_B 2 
uint8_t vfo_Display;
int m_band_select;
//rxtx vfo veriables
long Main_Display_FreqHz;
long Alt_Display_FreqHz;
String BandDisplay;
bool oneHz = false;


int m_avg;

#ifdef ILI9341_DISPLAY_X
#include "ILI9341_Display.h"
#define ILI9341_DISPLAY_H
ILI9341_DISPLAY ILI9341;
#endif

#ifdef MAX7219
#include "MAX7219_Display.h"
#define MAX7219_H
MAX7219_X MAX7219_X;
#endif

#ifdef DOT_MATRIX_LCD
//UNDER CONSTRUCTION!!!
#endif

#ifdef CHANNEL_SET
#define I2C_ENCODER
uint8_t maxnumberofChannels[6]{40,40,40,40,40,8};
String channelString;
const bool inverchanencDir = true; //to change encoder count direction
uint8_t chan_band_select= 1;
uint8_t m_chan_band_select;
uint8_t m_channelBand;
int num_of_chanBand = 6;
static String ChannelBand[6]{"LOW","FCC","MID","HIGH","UKFM","JAPAN"};
uint8_t channel_num_array[6]{1,1,1,1,1,1};//default channel
unsigned long prevmillis;
String chanBand;
bool m_chanelmodenabled = true;
bool m_m_chanelmodenabled;
bool chaninit;
int channel = 1;
int m_channel = 0;
uint8_t dis_chanNum;
bool sequencer;
#define chan_band_select_A  0
#define chan_band_select_B  1
#define chan_vfo_num        2
#define chan_mode_enabled   3
const uint8_t number_of_chan_gen_set = 4;
int chan_gen_set_mem_array[number_of_chan_gen_set]{0,0,1,0};
int m_chan_gen_set_mem_array[number_of_chan_gen_set];
int m_chan_band_select_A;
int m_chan_band_select_B;
const uint8_t num_of_chan_bands = 6;
uint8_t channel_num_array_A[num_of_chan_bands]{1,1,1,1,1,1};
uint8_t channel_num_array_B[num_of_chan_bands]{1,1,1,1,1,1};
uint8_t m_channel_num_array_A[num_of_chan_bands];
uint8_t m_channel_num_array_B[num_of_chan_bands];
uint8_t m_channel_num_A;
uint8_t m_channel_num_B;
//uint8_t chan_vfo_num = 1;
#endif
bool chanelmodenabled = false;
#ifdef I2C_ENCODER
#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>
#define SS_SWITCH        24
#define SS_NEOPIX        6
#define SEESAW_ADDR      0x36
Adafruit_seesaw ss;
seesaw_NeoPixel sspixel = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);
#endif
#define SDA0_Pin 40 //8   // select ESP32  I2C pins
#define SCL0_Pin 39 //9
ezButton button1(18);  // create ezButton object that attach to GPIO18; band up button
ezButton button2(17);  // create ezButton object that attach to GPIO17; band down button
ezButton button3(16);  // create ezButton object that attach to GPIO16; tuning step button
ezButton button4(15);  // create ezButton object that attach to GPIO15; encoder speed button
ezButton button5(8);  // create ezButton object that attach to GPIO8; vfo change button

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long m_displayFreqHz;
unsigned long long CLK1;
int modeShiftArray[5]{AM_ShiftHz, FM_ShiftHz, USB_ShiftHz, LSB_ShiftHz, CW_ShiftHz};
int modeshift=0;
int m_modeshift;
bool TX8mode = false;
bool m_TX8mode = true;
long tx8modeshift;
String rxtxmodeDisplay;
String band_display;
int tuning_step[4]{1,10,100,1000}; // tuning steps in Hz
String tuning_step_name[4]{"1Hz", "10Hz", "100Hz", "1KHz"};
long clk_gen_freq;
uint8_t speed[3]{20,8,1}; //
uint8_t m_speed;
int x_speed_select;
uint8_t speed_but_pin = 15;
uint8_t up_step_counter = 0;
uint8_t down_step_counter = 0;
uint8_t enc_dir;
uint8_t vfo_enc_clk = 36;
uint8_t vfo_enc_dir = 37;
uint8_t vfo_but_pin = 8;
unsigned long vfoPrevmillis;
bool vfosplitenabled = false;
bool m_vfosplitenabled = true;
bool vfo_release_inhib = false;



#ifdef VFO_LOCK
unsigned long lockPrevmillis;
bool vfolockenabled = false;
bool m_vfolockenabled = true;
bool vfounlocked = true;
String speed_name[4]{" SLOW"," MID"," FAST"," LOCK"};
#else
String speed_name[3]{" SLOW"," MID"," FAST"};
#endif

  uint8_t AM = 14;  //GPIO14
  uint8_t FM = 9;   //GPIO9
  uint8_t USB = 4;  //GPIO4
  uint8_t LSB = 3;  //GPIO3
  uint8_t CW = 2;   //GPIO2
  uint8_t TX8 = 1;  //GPIO1
  static int sense_pickuplev = 1000;    
  String ModeDisplay;
  String m_ModeDisplay;
  bool mode_condition[8]{};
  bool m_mode_condition[8]{};
  uint8_t modeselect[6]{AM,FM,USB,LSB,CW,TX8};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){

#ifdef FRAM_X
MB85RC256.begin(FRAM_I2C_address);
//FRAM_BACKUP
      for (int i = 0; i < number_of_bands; i++){      
      band_freqHz_A[i] = MB85RC256.init_band_freq_mem_A(reset, i, band_freqHz[i]);
      band_freqHz_B[i] = MB85RC256.init_band_freq_mem_B(reset, i, band_freqHz[i]); 
      m_band_freqHz_A[i] = band_freqHz_A[i];     
      }//end of for loop   
      for (int i = 0; i < number_of_gen_set; i++){
      gen_set_mem_array[i] = MB85RC256.init_general_global_settings(reset, i, gen_set_mem_array[i]);
      m_gen_set_mem_array[i] = gen_set_mem_array[i];
      }//end of for loop
#ifdef CHANNEL_SET
      for (int i = 0; i < num_of_chan_bands; i++){  
      channel_num_array_A[i] = MB85RC256.init_channel_set_A(reset, i,channel_num_array_A[i]);
      channel_num_array_B[i] = MB85RC256.init_channel_set_B(reset, i,channel_num_array_B[i]);
      m_channel_num_array_A[i] = channel_num_array_A[i];
      m_channel_num_array_B[i] = channel_num_array_B[i];
      }//end of loop
      for (int i = 0; i < number_of_chan_gen_set; i++){
      chan_gen_set_mem_array[i] = MB85RC256.init_chan_general_settings(reset, i, chan_gen_set_mem_array[i]);
      m_chan_gen_set_mem_array[i] = chan_gen_set_mem_array[i];
      }//end of loop
      if(chan_gen_set_mem_array[chan_mode_enabled]==1){
      chanelmodenabled = true;
      }else{chanelmodenabled = false;}
#endif

#else
  for (int i = 0; i < number_of_bands; i++) {
      band_freqHz_A[i] = band_freqHz[i];
      band_freqHz_B[i] = band_freqHz[i];
      }  
#endif

  Wire.begin(SDA0_Pin, SCL0_Pin);

  button1.setDebounceTime(50); // set debounce time in milliseconds, band up button
  button2.setDebounceTime(50); // set debounce time in milliseconds, band down button
  button3.setDebounceTime(50); // set debounce time in milliseconds, tuning step button
  button4.setDebounceTime(50); // set debounce time in milliseconds, encoder speed button
  button5.setDebounceTime(50); // set debounce time in milliseconds, vfo change button

  #ifdef SSD1306_DISPLAY_X
  SSD1306.begin(); 
  #endif

  #ifdef ILI9341_DISPLAY_X
  ILI9341.begin();  
  #endif
  
  pinMode(SQL_ADC_LEV,INPUT);/// Squelch pot
  pinMode(SQL_MUTE, OUTPUT);//Squelch mute

  pinMode(5,INPUT);/// Smeter test
  pinMode(36, INPUT); // enc Clock
  pinMode(37, INPUT); // enc Dir
  pinMode(speed_but_pin, INPUT_PULLUP); //
  pinMode(vfo_but_pin, INPUT_PULLUP);

  if(modeDisplayenable == true || modeShiftenable == true){
  pinMode(AM, INPUT);// AM Sense input pin
  pinMode(FM, INPUT);// FM Sense input pin
  pinMode(USB, INPUT);// USB Sense input pin
  pinMode(LSB, INPUT);// LSB Sense input pin
  pinMode(CW, INPUT);// CW Sense input pin
  }//end of if statement
  if(rxtxDisplayenable == true || TX8shiftenable == true){
  pinMode(TX8, INPUT);// TX8 Sense input pin
  }
  digitalPinToInterrupt(vfo_enc_clk);
  attachInterrupt(vfo_enc_clk, vfo_decoder, CHANGE);

  #ifdef MAX7219
  MAX7219_X.begin(LED_brightness);
  #endif  

  #ifdef DOT_MATRIX_LCD
  //UNDER CONSTRUCTION
  #endif

  #ifdef I2C_ENCODER
  sspixel.begin(SEESAW_ADDR);
  ss.begin(SEESAW_ADDR);
  sspixel.setBrightness(40);
  sspixel.show();
  ss.pinMode(SS_SWITCH, INPUT_PULLUP);
  ss.setGPIOInterrupts((uint32_t)1 << SS_SWITCH, 1);
  ss.enableEncoderInterrupt();
  ss.setEncoderPosition(0,0);
  #endif

  //BAND SELECTOR INIT
  #ifdef CHANNEL_SET
  band_freqHz_A[0] = channel_freq_gen(chan_gen_set_mem_array[chan_band_select_A], channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]]);
  band_freqHz_B[0] = channel_freq_gen(chan_gen_set_mem_array[chan_band_select_B], channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]]);
  #else 
  
  #endif
  //BAND SELECTOR INIT 
  updateSysStack +=1; 
  x_speed_select = gen_set_mem_array[speed_select];
 
  Serial.begin(115200);
  while(!Serial);

  #ifdef SI5351_X
  CLK_SI5351.begin(Cal_si5351);
  #endif

  #ifdef TIME_UTC
  UTC_TIMEX.begin(wifi_SSID, wifi_Password);
  #endif


}// end of setup
//****************************************setup*****************************************
//**************************************************************************************
//***************************************void_loop**************************************
void loop() {

#ifdef FRAM_X
      for (int i = 0; i < number_of_gen_set; i++){
      if(m_gen_set_mem_array[i] != gen_set_mem_array[i]){
      MB85RC256.general_global_settings(i, gen_set_mem_array[i]);
      m_gen_set_mem_array[i] = gen_set_mem_array[i];
      } }//end of for loop     
      MB85RC256.band_freq_mem_A(gen_set_mem_array[band_select_A],band_freqHz_A[gen_set_mem_array[band_select_A]]); 
      MB85RC256.band_freq_mem_B(gen_set_mem_array[band_select_B],band_freqHz_B[gen_set_mem_array[band_select_B]]);     

#ifdef CHANNEL_SET
      if(chanelmodenabled == true){
      chan_gen_set_mem_array[chan_mode_enabled] = 1;
      }else{chan_gen_set_mem_array[chan_mode_enabled] = 0;}

      for (int i = 0; i < number_of_chan_gen_set; i++){
      if(m_chan_gen_set_mem_array[i] != chan_gen_set_mem_array[i]){ //chan_gen_set_mem_array      
      MB85RC256.chan_general_settings(i, chan_gen_set_mem_array[i]);
      m_chan_gen_set_mem_array[i] = chan_gen_set_mem_array[i];
      }//end of if statement 
      }//end of for loop

      if(m_channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]] != channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]]){
      MB85RC256. channel_set_A(chan_gen_set_mem_array[chan_band_select_A], channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]]);           
      m_channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]] = channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]];
      }
      if(m_channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]] != channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]]){
      MB85RC256. channel_set_B(chan_gen_set_mem_array[chan_band_select_B], channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]]);
      m_channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]] = channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]];
      }

#endif
#endif


#ifdef CHANNEL_SET
Channelisation();
#endif

//mode sensing
    if(modeDisplayenable == true || modeShiftenable == true){//mode display and frequency shift controller
    uint8_t modeoff = 0;
    uint8_t modeerror = 0;
              for (uint8_t i = 0; i < 5; i++) { 
                    if(analogRead(modeselect[i]) > sense_pickuplev){
                      mode_condition[i] = true;
                      }else{mode_condition[i] = false;}
                      if(mode_condition[i] == false){modeoff ++;}
                      if(modeoff == 5){ModeDisplay = "   "; modeshift = 0;}
                      if(mode_condition[i] == true){modeerror ++;}
                      if(modeerror > 1){ModeDisplay = "ERR"; i = 5; modeshift = 0;}

                      if(mode_condition[i] == true && modeerror < 2 && modeShiftenable == true && modeShift_TX_enable == true){
                      modeshift = modeShiftArray[i];                  
                      }
                      if(mode_condition[i] == true && modeDisplayenable == true){         
                        switch(i){
                          case 0:            
                          ModeDisplay = "AM";
                          break;
                          case 1:
                          ModeDisplay = "FM";
                          break;
                          case 2:
                          ModeDisplay = "USB";
                          break;
                          case 3:
                          ModeDisplay = "LSB";
                          break;
                          case 4:
                          ModeDisplay = "CW";
                          break;
                        }
                      }
                    }//end of for loop
                  if(m_ModeDisplay != ModeDisplay){updateSysStack +=1;}
    }//end of if statement
      //mode sensing
      //RX TX sensing
      if(rxtxDisplayenable == true || TX8shiftenable == true){
        if(analogRead(TX8) > sense_pickuplev){
          TX8mode = true;
          rxtxmodeDisplay = "TX";
          if(TX8shiftenable == true){
          tx8modeshift = TX8_ShiftHz;}
        }else{tx8modeshift = 0; TX8mode = false; rxtxmodeDisplay = "RX";} 
        if(m_TX8mode != TX8mode){updateSysStack +=1;}
      }//end of if statement
      //RX TX sensing

      button1.loop(); // MUST call the loop() function first
      button2.loop(); // MUST call the loop() function first
      button3.loop(); // MUST call the loop() function first
      button4.loop(); // MUST call the loop() function first
      button5.loop(); // MUST call the loop() function first 

/////////////////////////////////////////////////
//BAND SELECTOR CONTROLLER      
      #ifdef CHANNEL_SET
      if(button1.isReleased()){//Band Up Button
      if(chanelmodenabled == true){
      if(chan_gen_set_mem_array[chan_vfo_num] == vfo_A){chan_gen_set_mem_array[chan_band_select_A] ++; if(chan_gen_set_mem_array[chan_band_select_A] > num_of_chanBand-1){chan_gen_set_mem_array[chan_band_select_A] = 0;}} 
      if(chan_gen_set_mem_array[chan_vfo_num] == vfo_B){chan_gen_set_mem_array[chan_band_select_B] ++; if(chan_gen_set_mem_array[chan_band_select_B] > num_of_chanBand-1){chan_gen_set_mem_array[chan_band_select_B] = 0;}}
      }else{
      if(gen_set_mem_array[vfo_num] == vfo_A){gen_set_mem_array[band_select_A] ++; if(gen_set_mem_array[band_select_A] > number_of_bands-1){gen_set_mem_array[band_select_A] = 1;} }
      if(gen_set_mem_array[vfo_num] == vfo_B){gen_set_mem_array[band_select_B] ++; if(gen_set_mem_array[band_select_B] > number_of_bands-1){gen_set_mem_array[band_select_B] = 1;} }
      } }
      if(button2.isReleased()){//Band Down Button
      if(chanelmodenabled == true){
      if(chan_gen_set_mem_array[chan_vfo_num] == vfo_A){chan_gen_set_mem_array[chan_band_select_A] --; if(chan_gen_set_mem_array[chan_band_select_A] < 0){chan_gen_set_mem_array[chan_band_select_A] = num_of_chanBand-1;}}
      if(chan_gen_set_mem_array[chan_vfo_num] == vfo_B){chan_gen_set_mem_array[chan_band_select_B] --; if(chan_gen_set_mem_array[chan_band_select_B] < 0){chan_gen_set_mem_array[chan_band_select_B] = num_of_chanBand-1;}}
      }else{ 
      if(gen_set_mem_array[vfo_num] == vfo_A){gen_set_mem_array[band_select_A] --; if(gen_set_mem_array[band_select_A] < 1){gen_set_mem_array[band_select_A] = number_of_bands-1;} }
      if(gen_set_mem_array[vfo_num] == vfo_B){gen_set_mem_array[band_select_B] --; if(gen_set_mem_array[band_select_B] < 1){gen_set_mem_array[band_select_B] = number_of_bands-1;} } 
      } }
      #else 
      if(button1.isReleased()){//Band Up Button
      if(gen_set_mem_array[vfo_num] == vfo_A){gen_set_mem_array[band_select_A] ++; if(gen_set_mem_array[band_select_A] > number_of_bands-1){gen_set_mem_array[band_select_A] = 1;} }
      if(gen_set_mem_array[vfo_num] == vfo_B){gen_set_mem_array[band_select_B] ++; if(gen_set_mem_array[band_select_B] > number_of_bands-1){gen_set_mem_array[band_select_B] = 1;} }            
      updateSysStack +=1;
      }
      if(button2.isReleased()){//Band Down Button
      if(gen_set_mem_array[vfo_num] == vfo_A){gen_set_mem_array[band_select_A] --; if(gen_set_mem_array[band_select_A] < 1){gen_set_mem_array[band_select_A] = number_of_bands-1;} }
      if(gen_set_mem_array[vfo_num] == vfo_B){gen_set_mem_array[band_select_B] --; if(gen_set_mem_array[band_select_B] < 1){gen_set_mem_array[band_select_B] = number_of_bands-1;} }           
      updateSysStack +=1;
      }            
      #endif
//BAND SELECTOR CONTROLLER
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//VFO AB CONTROLLER
#ifdef CHANNEL_SET
      if(chanelmodenabled == true){
      switch(chan_gen_set_mem_array[chan_vfo_num]){
      case 1:
      if(m_channel_num_A != channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]] || m_chan_band_select_A != chan_gen_set_mem_array[chan_band_select_A]){
      band_freqHz_A[0] = channel_freq_gen(chan_gen_set_mem_array[chan_band_select_A], channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]]);
      m_channel_num_A = channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]];
      m_chan_band_select_A = chan_gen_set_mem_array[chan_band_select_A];
      }// end of if statement
      Main_Display_FreqHz = band_freqHz_A[0];
      BandDisplay = ChannelBand[chan_gen_set_mem_array[chan_band_select_A]];
      Alt_Display_FreqHz = band_freqHz_B[0];      
      break;
      case 2:
      if(m_channel_num_B != channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]] || m_chan_band_select_B != chan_gen_set_mem_array[chan_band_select_B]){
      band_freqHz_B[0] = channel_freq_gen(chan_gen_set_mem_array[chan_band_select_B], channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]]);
      m_channel_num_B = channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]];
      m_chan_band_select_B = chan_gen_set_mem_array[chan_band_select_B];
      }// end of if statement
      Main_Display_FreqHz = band_freqHz_B[0];
      BandDisplay = ChannelBand[chan_gen_set_mem_array[chan_band_select_B]];
      Alt_Display_FreqHz = band_freqHz_A[0];
      break;
      }//end of switch case
      }else{
      switch(gen_set_mem_array[vfo_num]){
      case 1:
      Main_Display_FreqHz = band_freqHz_A[gen_set_mem_array[band_select_A]];
      BandDisplay = band_name[gen_set_mem_array[band_select_A]];
      Alt_Display_FreqHz = band_freqHz_B[gen_set_mem_array[band_select_B]];      
      break;
      case 2:            
      Main_Display_FreqHz = band_freqHz_B[gen_set_mem_array[band_select_B]];
      BandDisplay = band_name[gen_set_mem_array[band_select_B]];
      Alt_Display_FreqHz = band_freqHz_A[gen_set_mem_array[band_select_A]];
      break;
      }//end of switch case
      }//end of chanelmodenabled if statement
#else
      switch(gen_set_mem_array[vfo_num]){
      case 1:
      Main_Display_FreqHz = band_freqHz_A[gen_set_mem_array[band_select_A]];
      BandDisplay = band_name[gen_set_mem_array[band_select_A]];
      Alt_Display_FreqHz = band_freqHz_B[gen_set_mem_array[band_select_B]];      
      break;
      case 2:            
      Main_Display_FreqHz = band_freqHz_B[gen_set_mem_array[band_select_B]];
      BandDisplay = band_name[gen_set_mem_array[band_select_B]];
      Alt_Display_FreqHz = band_freqHz_A[gen_set_mem_array[band_select_A]];
      break;
      }//end of switch case
#endif
//VFO AB CONTROLLER
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//TUNING STEP CONTROLLER
    if(button3.isReleased()){//Tuning Step Button
    gen_set_mem_array[tuning_select] ++;    
    if(gen_set_mem_array[tuning_select] > 3){gen_set_mem_array[tuning_select]  = 0;}
    updateSysStack +=1;    
    }
//TUNING STEP CONTROLLER
/////////////////////////////////////////////////   
#ifdef VFO_LOCK
  //TUNING SPEED CONFIG
  if(digitalRead(speed_but_pin) == LOW ){//VFO Lock
      if(millis() - lockPrevmillis > 2000 && m_vfolockenabled != vfolockenabled){    
      vfolockenabled = true;
      vfounlocked = false;
      m_vfolockenabled = vfolockenabled;
      x_speed_select = 3;
      updateSysStack +=1;
      }
  }else{lockPrevmillis = millis();}  

  if(vfolockenabled == false){    
    if(button4.isReleased()){//Speed Button
      if(vfounlocked == true){    
      gen_set_mem_array[speed_select] ++;      
      if(gen_set_mem_array[speed_select] > 2){gen_set_mem_array[speed_select]  = 0;}
      }
      vfounlocked = true;
      x_speed_select = gen_set_mem_array[speed_select];
      updateSysStack +=1;
      }
    }
 
  if(button4.isPressed() && m_vfolockenabled == vfolockenabled){
  vfolockenabled = false;  
  }
#else
if(button4.isReleased()){//Speed Button        
      gen_set_mem_array[speed_select] ++;      
      if(gen_set_mem_array[speed_select] > 2){gen_set_mem_array[speed_select]  = 0;}           
      x_speed_select = gen_set_mem_array[speed_select];
      updateSysStack +=1;
      }
  //TUNING SPEED CONFIG      
#endif

/////////////////////////////////////////////////
//VFO SELECTOR CONFIG
#ifdef CHANNEL_SET 
          if(chanelmodenabled == true){ 
            if(digitalRead(vfo_but_pin) == LOW ){//VFO Lock
            if(millis() - vfoPrevmillis > 2000 && m_vfosplitenabled != vfosplitenabled){    
            vfosplitenabled =! vfosplitenabled;      
            m_vfosplitenabled = vfosplitenabled;
            vfo_release_inhib = true;    
            updateSysStack +=1;
            }
            }else{vfoPrevmillis = millis();}
            if(button5.isReleased()){//Vfo Change Button
            m_vfosplitenabled =! vfosplitenabled;
            if(vfo_release_inhib == false){chan_gen_set_mem_array[chan_vfo_num] ++;}    
            if(chan_gen_set_mem_array[chan_vfo_num] > 2){chan_gen_set_mem_array[chan_vfo_num]  = 1;} 
            updateSysStack +=1;
            vfo_release_inhib = false;
            }
            vfo_Display = chan_gen_set_mem_array[chan_vfo_num];
            }else{
            if(digitalRead(vfo_but_pin) == LOW ){//VFO Lock
            if(millis() - vfoPrevmillis > 2000 && m_vfosplitenabled != vfosplitenabled){    
            vfosplitenabled =! vfosplitenabled;      
            m_vfosplitenabled = vfosplitenabled;
            vfo_release_inhib = true;    
            updateSysStack +=1;
            }
            }else{vfoPrevmillis = millis();}
            if(button5.isReleased()){//Vfo Change Button
            m_vfosplitenabled =! vfosplitenabled;
            if(vfo_release_inhib == false){gen_set_mem_array[vfo_num] ++;}    
            if(gen_set_mem_array[vfo_num] > 2){gen_set_mem_array[vfo_num]  = 1;} 
            updateSysStack +=1;
            vfo_release_inhib = false;
            }
            vfo_Display = gen_set_mem_array[vfo_num];
          }//end of chanelmodenabled if statement
#else
           if(digitalRead(vfo_but_pin) == LOW ){//VFO Lock
           if(millis() - vfoPrevmillis > 2000 && m_vfosplitenabled != vfosplitenabled){    
           vfosplitenabled =! vfosplitenabled;      
           m_vfosplitenabled = vfosplitenabled;
           vfo_release_inhib = true;    
           updateSysStack +=1;
           }
           }else{vfoPrevmillis = millis();}
           if(button5.isReleased()){//Vfo Change Button
           m_vfosplitenabled =! vfosplitenabled;
           if(vfo_release_inhib == false){gen_set_mem_array[vfo_num] ++;}    
           if(gen_set_mem_array[vfo_num] > 2){gen_set_mem_array[vfo_num]  = 1;} 
           updateSysStack +=1;
           vfo_release_inhib = false;
           }
          vfo_Display = gen_set_mem_array[vfo_num];  
#endif
//VFO SELECTOR CONFIG
/////////////////////////////////////////////////


              uint8_t oneHzRes;   
              int decimal_places;
              if(gen_set_mem_array[tuning_select] == 0){
              oneHz = true;
              oneHzRes = 1;
              }else{oneHzRes = 0; oneHz = false;}
        

 ////////////////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////////////////

             

              if(updateSysStack > 0 || 1==1){ // FOR TESTING!!!!
              //Serial.println("KNOCK!!");                   

              #ifdef ILI9341_DISPLAY_X
              #ifdef CHANNEL_SET
              if(vfosplitenabled == true && TX8mode == true){
              ILI9341.FreqDisplay(chanelmodenabled, oneHz, Alt_Display_FreqHz);  
              }else{                       
              ILI9341.FreqDisplay(chanelmodenabled, oneHz, Main_Display_FreqHz);} 
              ILI9341.ChannelDisplay(chanelmodenabled, channel);
              #else
              if(vfosplitenabled == true && TX8mode == true){
              ILI9341.FreqDisplay(false, oneHz, Alt_Display_FreqHz);  
              }else{
              ILI9341.FreqDisplay(false, oneHz, Main_Display_FreqHz);}
              #endif
              ILI9341.altvfofreq(Alt_Display_FreqHz); 
              ILI9341.vfoDisplay(vfo_Display); 
              ILI9341.modeDisplay(ModeDisplay);
              ILI9341.rxtxDisplay(TX8mode);
              ILI9341.tuningstep(gen_set_mem_array[tuning_select]);
              ILI9341.tuningspeed(x_speed_select);
              ILI9341.band_display(chanelmodenabled, BandDisplay);
              ILI9341.vfoRxtx(vfo_split_controller(vfosplitenabled, vfo_Display));
              ILI9341.signal_bargraph(TX8mode, signal_linearizer(ADC_Sig_Conditioner(SIG_ADC_LEV)),sql_enable, ADC_Sig_Conditioner(SQL_ADC_LEV),
              Squelch_CTL(signal_linearizer(ADC_Sig_Conditioner(SIG_ADC_LEV)), ADC_Sig_Conditioner(SQL_ADC_LEV), sql_enable));
              ILI9341.freq_incdisplay(oneHz, Main_Display_FreqHz, BandDisplay, chanelmodenabled);
              #ifdef TIME_UTC
              ILI9341.utcTime(UTC_TIMEX.printLocalTime()); 
              #endif
              #endif

              #ifdef SSD1306_DISPLAY_X              
              SSD1306.dispaly_freq(Alt_Display_FreqHz, Main_Display_FreqHz, TX8mode, vfosplitenabled, chanelmodenabled);
              SSD1306.display_freq_res(oneHzRes, oneHz);
              SSD1306.display_tstep_speed(tuning_step_name[gen_set_mem_array[tuning_select]], speed_name[x_speed_select]);
              SSD1306.display_band_mode(BandDisplay, ModeDisplay, TX8mode, modeDisplayenable, rxtxDisplayenable);
              SSD1306.display_vfo(vfo_Display);
              #ifdef CHANNEL_SET
              SSD1306.display_channel_mode(chanelmodenabled, channel, channel);              
              #endif
              #endif

              #ifdef MAX7219
              MAX7219_X.MAX7219_display(Main_Display_FreqHz, Alt_Display_FreqHz, vfosplitenabled);
              #endif





                 
                
                  m_ModeDisplay = ModeDisplay;//leave in
                  m_TX8mode = TX8mode; //leave in

                  updateSysStack -=1;

                  #ifdef CHANNEL_SET
                  m_channel = channel_num_array[chan_band_select];
                  m_channelBand = chan_band_select;
                  m_m_chanelmodenabled = chanelmodenabled; 
                  #endif


            }//end of update stack  if statement FOR TESTING!!!!

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// Split VFO frequency controler
if(vfosplitenabled == true && TX8mode == true){clk_gen_freq = ((Alt_Display_FreqHz * up_down_conversion) + IF_freqHz + modeshift + tx8modeshift);}else{
clk_gen_freq = ((Main_Display_FreqHz * up_down_conversion) + IF_freqHz + modeshift + tx8modeshift);}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////



#ifdef SI5351_X
CLK_SI5351.CLK0_FreqGen(clk_gen_freq);
#endif


}// end of void loop
//***************************************void_loop**************************************
//**************************************************************************************
//**************************************vfo_decoder*************************************

void vfo_decoder(void) {
    #ifdef VFO_LOCK
    if(vfolockenabled == false){
    #endif
    if(TX8mode == false){// TX VFO inhibit
    up_step_counter ++;
    down_step_counter ++;
    if(m_speed != speed[gen_set_mem_array[speed_select]]){ // added to stop encoder lag when switch for slow to fast speeds
        up_step_counter = 0;
        down_step_counter = 0;
        m_speed = speed[gen_set_mem_array[speed_select]];  
      }
    if(invert_encoder_direction == true){enc_dir =0;}else{enc_dir =1;}    

      if (digitalRead(vfo_enc_dir) == enc_dir){
          down_step_counter =0;
          if(up_step_counter == speed[gen_set_mem_array[speed_select]]){
          #ifdef CHANNEL_SET
          if(chan_gen_set_mem_array[chan_vfo_num] == vfo_A && chanelmodenabled == true){band_freqHz_A[0]+=tuning_step[gen_set_mem_array[tuning_select]];}
          if(chan_gen_set_mem_array[chan_vfo_num] == vfo_B && chanelmodenabled == true){band_freqHz_B[0]+=tuning_step[gen_set_mem_array[tuning_select]];}
          #endif
          if(gen_set_mem_array[vfo_num] == vfo_A && chanelmodenabled == false){band_freqHz_A[gen_set_mem_array[band_select_A]]+=tuning_step[gen_set_mem_array[tuning_select]];}
          if(gen_set_mem_array[vfo_num] == vfo_B && chanelmodenabled == false){band_freqHz_B[gen_set_mem_array[band_select_B]]+=tuning_step[gen_set_mem_array[tuning_select]];}
          up_step_counter = 0;
          }
          }else{
          up_step_counter = 0;
          if(down_step_counter == speed[gen_set_mem_array[speed_select]]){
          #ifdef CHANNEL_SET 
          if(chan_gen_set_mem_array[chan_vfo_num] == vfo_A && chanelmodenabled == true){band_freqHz_A[0]-=tuning_step[gen_set_mem_array[tuning_select]];}
          if(chan_gen_set_mem_array[chan_vfo_num]== vfo_B && chanelmodenabled == true){band_freqHz_B[0]-=tuning_step[gen_set_mem_array[tuning_select]];}
          #endif
          if(gen_set_mem_array[vfo_num] == vfo_A && chanelmodenabled == false){band_freqHz_A[gen_set_mem_array[band_select_A]]-=tuning_step[gen_set_mem_array[tuning_select]];}
          if(gen_set_mem_array[vfo_num] == vfo_B && chanelmodenabled == false){band_freqHz_B[gen_set_mem_array[band_select_B]]-=tuning_step[gen_set_mem_array[tuning_select]];}
          down_step_counter = 0;
          }
      } //end of digital read encoder if statement

    }//end of TX8mode if statement

  #ifdef VFO_LOCK
  }// end of vfolockenabled if statment
  #endif
  updateSysStack +=1;
}// end of vfo_decoder function
//**************************************vfo_decoder*************************************
//**************************************************************************************

//*************************************************************************
//*************************************************************************
#ifdef CHANNEL_SET
void Channelisation(){
//if(m_chanelmodenabled != chanelmodenabled){
//}  

    if(millis() - prevmillis > 30){//Timer 60ms delay 60
    sequencer =! sequencer;
      if(sequencer == true){  
          if (ss.digitalRead(SS_SWITCH) == LOW && m_chanelmodenabled != chanelmodenabled) {
              chanelmodenabled =! chanelmodenabled;        
              m_chanelmodenabled = chanelmodenabled;
              updateSysStack +=1;                       
              }else{
                    if (ss.digitalRead(SS_SWITCH) == HIGH){
                    m_chanelmodenabled =! chanelmodenabled;
                    updateSysStack +=1;                    
                      }
            }// end of chanelmodenabled toggle if statment


         } //end of sequencer  if statement
    if(sequencer == false){
          if(chanelmodenabled == true){          
            if(ss.getEncoderPosition() !=0){
                if(ss.getEncoderPosition() > 0){

                if(chan_gen_set_mem_array[chan_vfo_num] == vfo_A){//VFO_A                
                if(inverchanencDir == false){channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]] ++;}else{channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]] --;}
                if(channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]] < 1){channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]] = maxnumberofChannels[chan_gen_set_mem_array[chan_band_select_A]];}
                if(channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]] > maxnumberofChannels[chan_gen_set_mem_array[chan_band_select_A]]){ channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]] = 1;}
                }//end of VFO_A if statement
                if(chan_gen_set_mem_array[chan_vfo_num] == vfo_B){//VFO_B               
                if(inverchanencDir == false){channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]] ++;}else{channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]] --;}
                if(channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]] < 1){channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]] = maxnumberofChannels[chan_gen_set_mem_array[chan_band_select_B]];}
                if(channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]] > maxnumberofChannels[chan_gen_set_mem_array[chan_band_select_B]]){ channel_num_array_A[chan_gen_set_mem_array[chan_band_select_B]] = 1;}
                }//end of VFO_B if statement                 
                ss.setEncoderPosition(0,0);                
                }// channel encoder up-counter if statement
                if(ss.getEncoderPosition() < 0){                
                if(chan_gen_set_mem_array[chan_vfo_num] == vfo_A){//VFO_A                
                if(inverchanencDir == false){channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]] --;}else{channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]] ++;}

                if(channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]] < 1){channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]] = maxnumberofChannels[chan_gen_set_mem_array[chan_band_select_A]];}
                if(channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]] > maxnumberofChannels[chan_gen_set_mem_array[chan_band_select_A]]){ channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]] = 1;}
                band_freqHz_A[0] = channel_freq_gen(chan_gen_set_mem_array[chan_band_select_A], channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]]);
                }//end of VFO_A if statement
                if(chan_gen_set_mem_array[chan_vfo_num] == vfo_B){//VFO_B               
                if(inverchanencDir == false){channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]] --;}else{channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]] ++;}

                if(channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]] < 1){channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]] = maxnumberofChannels[chan_gen_set_mem_array[chan_band_select_B]];} 
                if(channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]] > maxnumberofChannels[chan_gen_set_mem_array[chan_band_select_B]]){ channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]] = 1;}               
                band_freqHz_B[0] = channel_freq_gen(chan_gen_set_mem_array[chan_band_select_B], channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]]);
                } //end of VFO_B if statement                           
                ss.setEncoderPosition(0,0);                
                }// channel encoder down-counter if statement
                updateSysStack +=1;
              }// end of getEncoderPosition() !=0 if statement
                      
              if(chanelmodenabled == true){  
              if(chan_gen_set_mem_array[chan_vfo_num] == vfo_A){channel = channel_num_array_A[chan_gen_set_mem_array[chan_band_select_A]] ;}
              if(chan_gen_set_mem_array[chan_vfo_num] == vfo_B){channel = channel_num_array_B[chan_gen_set_mem_array[chan_band_select_B]] ;} 
              }                  

              }else{chaninit = false;} //end of chanelmodenabled if statement
          }//end of sequencer if statement
      prevmillis = millis();  
      }//end of timmer if statement 
} // end of Channelisation function
//*************************************************************************
//*************************************************************************
long channel_freq_gen(uint8_t chanBand, uint8_t chanNum){
unsigned long freq;
static long fccChanbw = 450000;
static long UKFMstartfreq = 27601250;
unsigned long FCCstartfreq = 26965000;
int JapanCBfreq[9]{0,26968,26976,27040,27080,27088,27112,27120,27144};

  if(chanBand < 4){ // FCC Channel freq generator
      switch (chanNum){
          case 4 ... 7:
            FCCstartfreq += 10e3;
            break;
          case 8 ... 11:
            FCCstartfreq += 20e3;
            break;       
          case 12 ... 15:
            FCCstartfreq += 30e3;
            break;
          case 16 ... 19:
            FCCstartfreq += 40e3;
            break;       
          case 20 ... 22:
            FCCstartfreq += 50e3;
            break;        
          case 23:
            FCCstartfreq += 70e3;
            break;
          case 24 ... 25:
          FCCstartfreq += 40e3;
            break;        
          case 26 ... 40:
          FCCstartfreq += 50e3;
            break;
        }//end of switch case
  }//end fo chanband if statement
          switch(chanBand){
            case 0:
            freq = (FCCstartfreq - fccChanbw) + ((chanNum -1) * 10e3);
            break;
            case 1:
            freq = FCCstartfreq + ((chanNum -1) * 10e3);
            break;
            case 2:
            freq = FCCstartfreq + fccChanbw  + ((chanNum -1) * 10e3);
            break; 
            case 3:
            freq = FCCstartfreq + (fccChanbw *2) + ((chanNum -1) * 10e3);
            break;
            case 4:
            freq = UKFMstartfreq + ((chanNum -1) * 10e3);
            break;
            case 5:
            freq = JapanCBfreq[chanNum] *1e3;
            break;
          }
return freq;
}//end of channel_freq_gen
#endif//CHANNEL_SET
//*************************************************************************
//*************************************************************************
int ADC_Sig_Conditioner(uint8_t ADC){
      int sample_rate = 20;
      int avg = 0;
      long whole_num = 0;
      uint8_t differential = 10;
      for (int i = 0; i <= sample_rate; i++){
      whole_num += analogRead(ADC);
      }// end of for loop
      avg = whole_num / sample_rate;
      if(m_avg != avg && avg - m_avg > differential || avg - m_avg < differential *-1){
      m_avg = avg; 
      }
      return m_avg;
}//end of A2D_Sig_conditioner fuction
//*************************************************************************
//*************************************************************************
int signal_linearizer(int sig_lev){
float sig_lev_float;
int meter_lev;
if(sig_lev < 110){meter_lev = 0;}

  //UNDER CONSTRUCTION!!!
//int req_lev[17]{0,135, 473, 723, 937, 1108, 1356, 1605, 1826, 2051, 2356, 2589, 2842, 3012, 3254, 3474, 3756};
//int act_lev[17]{0, 50, 95, 105, 121, 205, 425, 823, 1230, 1560, 1779, 2060, 2234, 2358, 2461, 2576, 2732};

sig_lev_float = sig_lev * bar_meter_lev;//1.52//bar_meter_lev
meter_lev = sig_lev_float;






return meter_lev;
}//end of function
//*************************************************************************
//*************************************************************************
bool sql = false;
bool Squelch_CTL(int meter_lev, int sql_lev, bool sql_enable){
//UNDER CONSTRUCTION!!!
          uint8_t sql_hyst = 100;
          int sql_off_delay = 100;
          if(sql_enable == true){
          if(sql_lev > meter_lev && sql == false){
          sql = true;
          digitalWrite(SQL_MUTE, HIGH);
          sql_millis = millis();
          }
          if(sql_lev + sql_hyst < meter_lev && millis() - sql_millis > sql_off_delay){
          sql = false;
          digitalWrite(SQL_MUTE, LOW);
          }
          }else{sql = false;}// end of sql_enable if statement
return sql;
}// end of Squelch_CTL function
//*************************************************************************
//*************************************************************************
int vfo_split_controller(bool vfosplitenabled, uint8_t vfo_Display){
uint8_t vfo_split;
          if(vfosplitenabled == true){
              switch(vfo_Display){
              case 1:
              vfo_split = 1;
              break;
              case 2:
              vfo_split = 3;
              break;
              }//end of switch
            }else{                    
              switch(vfo_Display){
              case 1:
              vfo_split = 0;
              break;
              case 2:
              vfo_split = 2;
              break;
            }//end of switch
          }//end of if
return vfo_split;
}// end of vfo split fuction
//*************************************************************************
//*************************************************************************
