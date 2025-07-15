/*
 * SI5351_VFO_002.ino - Simple VFO sketch with IF offsets and displays.
 * mode and TX sensing with frequency shifting options added, band counter fixed and encoder lag issue fixed                          
 * 
 * Copyright 2024 John Hawkes (M0XFX) <m0xfx.channel@gmail.com>
 *                       
 * 
 *
 * This sketch  is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 002 of the License, or
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
#include <si5351.h>
#include <ezButton.h>

//********************************************************************************************************************************************************************************************
//********************************************************************************************************************************************************************************************
//********************************************************************************************************************************************************************************************
//#define MAX7219
#define U8G2DISPLAY
//#define CLK1_ENABLE

int band_select = 0; //Default band at startup
uint8_t LED_brightness = 8; //set LED brightness range 1 - 8

//CLK0
long IF_freqHz = 10695000; //IF frequency in Hz, set to 0 if IF shift is not required.

const uint8_t number_of_bands = 16;
long band_freqHz[number_of_bands]{1116300, 1800000, 3500000, 5351000, 7000000, 10100000, 14000000, 18068000, 21000000, 24890000, 27000000, 28000000, 50000000, 70000000, 88000000, 144000000};
String band_name[number_of_bands] = {"MW", "160M", "80M", "60M", "40M", "30M", "20M", "17M", "15M", "12M", "11M", "10M", "6M", "4M", "FM", "2M"};

int up_down_conversion = 1;// 1 = down conversion, -1 = up conversion mixer
bool invert_encoder_direction = false; // change direction of rotary encoder counting true/false 

const long Cal_si5351 = 122360;//122360; // need to go through calibration routine to get this figure

static bool modeDisplayenable = false; //Display mode selected
static bool modeShiftenable = false; //Enables mode frequency shifting
int AM_ShiftHz = 0; //Frequency shift in Hz for AM mode, can be positive or negative value
int FM_ShiftHz = 0; //Frequency shift in Hz for FM mode, can be positive or negative value
int USB_ShiftHz = 0; //Frequency shift in Hz for USB mode, can be positive or negative value
int LSB_ShiftHz = 0; //Frequency shift in Hz for LSB mode, can be positive or negative value
int CW_ShiftHz = 0; //Frequency shift in Hz for CW mode, can be positive or negative value

static bool rxtxDisplayenable = false; //Displays RX/TX mode
static bool TX8shiftenable = false; //Enables mode TX shifting
static long TX8_ShiftHz = 0; //Frequency shift in Hz for TX mode, can be positive or negative value

static bool modeShift_TX_enable = false; //Enables mode frequency shifting in TX mode as well as RX 
//********************************************************************************************************************************************************************************************
//********************************************************************************************************************************************************************************************
//********************************************************************************************************************************************************************************************
#ifdef U8G2DISPLAY
#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
#endif

#ifdef MAX7219
#include <LedControl.h>
LedControl lc=LedControl(12,11,10,0);
#endif

ezButton button1(9);  // create ezButton object that attach to pin 9; band up button
ezButton button2(8);  // create ezButton object that attach to pin 8; band down button
ezButton button3(7);  // create ezButton object that attach to pin 7; tuning step button
ezButton button4(6);  // create ezButton object that attach to pin 6; encoder speed button


float displayFreqMHz;
long m_displayFreqHz;
unsigned long long CLK1;
int modeShiftArray[5]{AM_ShiftHz, FM_ShiftHz, USB_ShiftHz, LSB_ShiftHz, CW_ShiftHz};

int modeshift=0;
int m_modeshift;
bool TX8mode = false;
bool m_TX8mode;
long tx8modeshift;
String rxtxmodeDisplay;

String band_display;
//String band_string = "  BAND";
int tuning_step[4]{1,10,100,1000}; // tuning steps in Hz
uint8_t tuning_select = 2;
uint8_t m_tuning_select;
String tuning_step_name[4]{"1Hz", "10Hz", "100Hz", "1KHz"};
String tuning_step_display;
String step_string = "TS ";
String freq_inc_name[2]{"KHz", "MHz"};
int freq_inc_select;
unsigned long long clk_gen_freq;
bool freq_range[3]{true,true,true};
long freq_rangeHz[3]{1e6,10e6,100e6};
uint8_t speed[3]{20,8,1}; //
uint8_t speed_select = 2; // 2 defalt
uint8_t m_speed_select; 
String speed_name[3]{" SLOW"," MID"," FAST"};
uint8_t up_step_counter = 0;
uint8_t down_step_counter = 0;
uint8_t enc_dir;

//Clock gen frequency limiter
//long clk_gen_freq_max_Hz = 200e6; //max limit frequency in Hz for clock generaotor, Note: 999999999 will apaer on OLED when max is reached. 
int clk_gen_freq_min_Hz = 8e3; //min limit frequency in Hz for clock generaotor, Note: 00000000 will apaer on OLED when max is reached.

Si5351 si5351;

  uint8_t AM = A7;
  uint8_t FM = A6;
  uint8_t USB = A3;
  uint8_t LSB = A2;
  uint8_t CW = A1;
  uint8_t TX8 = A0;
  static int sense_pickuplev = 169;    
  String ModeDisplay;
  String m_ModeDisplay;
  bool mode_condition[8]{};
  bool m_mode_condition[8]{};
  uint8_t modeselect[6]{AM,FM,USB,LSB,CW,TX8};

void setup() {
  button1.setDebounceTime(100); // set debounce time in milliseconds, band up button
  button2.setDebounceTime(100); // set debounce time in milliseconds, band down button
  button3.setDebounceTime(100); // set debounce time in milliseconds, tuning step button
  button4.setDebounceTime(100); // set debounce time in milliseconds, encoder speed button
  #ifdef U8G2DISPLAY
  u8g2.setI2CAddress(0x78); // I2C address for OLED
  u8g2.begin();
  #endif
  pinMode(2, INPUT); 
  pinMode(3, INPUT);
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
  attachInterrupt(0, vfo_decoder, CHANGE);
  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
  si5351.set_correction(Cal_si5351, SI5351_PLL_INPUT_XO); //calibration for si5351 //122360
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA); //RF output level adjust
  #ifdef CLK1_ENABLE
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA); //RF output level adjust
  si5351.set_freq(102400000, SI5351_CLK1); //To set clock 1 add frequncy in Hz plus two zeros
  #endif  
  si5351.update_status();

  #ifdef MAX7219
//MAX72XX////////////////////////////////////
  lc.shutdown(0,false); //The MAX72XX is in power-saving mode on startup, we have to do a wakeup call
  lc.setIntensity(0, LED_brightness); //Set the brightness to a medium values
  lc.clearDisplay(0); //and clear the display
//MAX72XX////////////////////////////////////  
  #endif
  //Serial.begin(9600);

}// end of setup


void loop() {

if(modeDisplayenable == true || modeShiftenable == true){//mode display and frequency shift controller
uint8_t modeoff = 0;
uint8_t modeerror = 0;
          for (uint8_t i = 0; i < 5; i++) { 
                if(analogRead(modeselect[i]) > sense_pickuplev){
                  mode_condition[i] = true;
                  }else{mode_condition[i] = false;}
                  if(mode_condition[i] == false){modeoff ++;}
                  if(modeoff == 5){ModeDisplay = " "; modeshift = 0;}
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
}//end of if statement


if(rxtxDisplayenable == true || TX8shiftenable == true){

  if(analogRead(TX8) > sense_pickuplev){
    TX8mode = true;
    rxtxmodeDisplay = "TX";
    if(TX8shiftenable == true){
    tx8modeshift = TX8_ShiftHz;
    }
  }else{tx8modeshift = 0; TX8mode = false; rxtxmodeDisplay = "RX";} 

}//end of if statement

  button1.loop(); // MUST call the loop() function first
  button2.loop(); // MUST call the loop() function first
  button3.loop(); // MUST call the loop() function first
  button4.loop(); // MUST call the loop() function first

  if(button1.isReleased()){//Band Up Button
    band_select ++;    
    if(band_select > number_of_bands-1){band_select = 0;}
    }
  if(button2.isReleased()){//Band Down Button
    band_select --;
    if(band_select < 0){band_select = number_of_bands -1;}    
    }
  if(button3.isReleased()){//Tuning Step Button
    tuning_select ++;    
    if(tuning_select > 3){tuning_select  = 0;}
    }
  if(button4.isReleased()){//Speed Button
    speed_select ++;    
    if(speed_select > 2){speed_select  = 0;}
    }    

  uint8_t oneHzRes;
  uint8_t decimal_places;

      if(tuning_select == 0){
        oneHzRes = 1;
        }else{oneHzRes = 0;}
        

          if(m_displayFreqHz != band_freqHz[band_select] || m_tuning_select != tuning_select || m_speed_select != speed_select || m_ModeDisplay != ModeDisplay || m_TX8mode != TX8mode ){

            if(band_freqHz[band_select] < 1e6){
             displayFreqMHz = (float) band_freqHz[band_select] / 1e3;  //1e6;
             freq_inc_select=0;
             decimal_places = 3 + oneHzRes;

              }else{
                displayFreqMHz = (float) band_freqHz[band_select] / 1e6;
                freq_inc_select=1;
                decimal_places = 5 + oneHzRes;
                }
                if(modeDisplayenable == true){band_display = band_name[band_select] + " " + ModeDisplay;}else{band_display = band_name[band_select] + " BAND";}
                
                tuning_step_display = step_string + tuning_step_name[tuning_select] + speed_name[speed_select];

                 #ifdef U8G2DISPLAY
                  u8g2.firstPage();
                  do {
                      u8g2.setFont(u8g2_font_smart_patrol_nbp_tf);
                      u8g2.setCursor(0, 10);
                      u8g2.print (band_display);
                      u8g2.setCursor(100, 10);
                      if(rxtxDisplayenable == true){
                      u8g2.print (rxtxmodeDisplay); //freq_inc_name[freq_inc_select] //rxtxmodeDisplay //ModeDisplay
                      }else{
                      u8g2.print (freq_inc_name[freq_inc_select]);
                      }
                      u8g2.setCursor(0, 64);
                      u8g2.print (tuning_step_display); //tuning_step_name                    
                      u8g2.setFont(u8g2_font_logisoso24_tn);
                      u8g2.setCursor(0, 45); //40 
                      u8g2.print (displayFreqMHz, decimal_places); 
                      } while ( u8g2.nextPage() );
                      
                 #endif                

                // Clock generator frequency limiter start
                  if(((band_freqHz[band_select] * up_down_conversion) + IF_freqHz)<= clk_gen_freq_min_Hz){ 
                      #ifdef U8G2DISPLAY
                      u8g2.firstPage();
                          do {
                              u8g2.setFont(u8g2_font_logisoso24_tn);
                              u8g2.setCursor(0, 45); //40 
                              u8g2.print ("00000000");
                              } while ( u8g2.nextPage() );
                      #endif        
                      clk_gen_freq = clk_gen_freq_min_Hz * 1e2;
                    }

                  if(((band_freqHz[band_select] * up_down_conversion) + IF_freqHz)>= 200e6){
                      #ifdef U8G2DISPLAY
                        u8g2.firstPage();
                            do {
                                u8g2.setFont(u8g2_font_logisoso24_tn);
                                u8g2.setCursor(0, 45); //40 
                                u8g2.print ("99999999");
                                } while ( u8g2.nextPage() );
                      #endif          
                        clk_gen_freq = 200e6 * 1e2;
                      }
                  // Clock generator frequency limiter end
                

                  clk_gen_freq = (unsigned long long) ((band_freqHz[band_select] * up_down_conversion) + IF_freqHz + modeshift + tx8modeshift) * 1e2; 
                  si5351.set_freq(clk_gen_freq, SI5351_CLK0); //1400000000ULL
                  m_displayFreqHz = band_freqHz[band_select];
                  m_tuning_select = tuning_select;
                  m_speed_select = speed_select;
                  m_ModeDisplay = ModeDisplay;
                  m_TX8mode = TX8mode;
                  #ifdef MAX7219
                  SevenSegDisplay();
                  #endif

            }//end if IF statment



}// end of void loop



void vfo_decoder(void) {
    up_step_counter ++;
    down_step_counter ++;

    if(invert_encoder_direction == true){enc_dir =0;}else{enc_dir =1;}

      if (digitalRead(3) == enc_dir){
          down_step_counter =0;
          if(up_step_counter == speed[speed_select]){
          band_freqHz[band_select] +=tuning_step[tuning_select];
          up_step_counter =0;
          }

          }else{
          up_step_counter =0;
          if(down_step_counter == speed[speed_select]){
          band_freqHz[band_select] -=tuning_step[tuning_select];
          down_step_counter =0;
          }
      }  

   
}// end of vfo_decoder function

#ifdef MAX7219
void SevenSegDisplay() {
bool point_marker []{false, false, false, false, false, false, false, false, false }; 
long m_freqHz;
          switch(band_freqHz[band_select]){
                    case 1000 ... 999999: //KHz
                      point_marker [2] = true;                                                      
                      break;
                    case 1000000 ... 1000000000: //MHz
                      point_marker [5] = true;                                       
                      break;
              }

        if(band_freqHz[band_select] < freq_rangeHz[0]){
          if(freq_range[0] == true){ 
            lc.clearDisplay(0); freq_range[0] =! freq_range[0];} 
            }else{freq_range[0] = true;}

        if(band_freqHz[band_select] < freq_rangeHz[1]){
          if(freq_range[1] == true){ 
            lc.clearDisplay(0); freq_range[1] =! freq_range[1];} 
            }else{freq_range[1] = true;} 

        if(band_freqHz[band_select] < freq_rangeHz[2]){
          if(freq_range[2] == true){ 
            lc.clearDisplay(0); freq_range[2] =! freq_range[2];} 
            }else{freq_range[2] = true;}         

int j = -1;
m_freqHz = band_freqHz[band_select];

              while (m_freqHz > 0){
                   int digit = m_freqHz %10;
                   m_freqHz  /= 10;                    
                   lc.setChar(0,j,digit,point_marker [j]);                   
                   j++;
                }

} // end of function
#endif
