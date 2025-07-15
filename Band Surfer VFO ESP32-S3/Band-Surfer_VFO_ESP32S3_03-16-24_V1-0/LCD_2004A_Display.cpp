#include "LCD_2004A_Display.h"
//UNDER CONSTRUCTION!!!!



//LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display



//void LCD_2004A_DISPLAY::begin(void){
 // lcd.begin(); 
  // lcd.backlight();
//}// end of begin function


/*
                           #ifdef DOT_MATRIX_LCD
                    String freqstring;
                    freqstring = String(displayFreqMHz, decimal_places);                       

                    if(m_displayFreqHz != Main_Display_FreqHz || m_ModeDisplay != ModeDisplay || m_tuning_select != gen_set_mem_array[tuning_select]){
                    lcd.setCursor(0,0);
                    lcd.print(freqstring + " " + freq_inc_name[freq_inc_select] + " " + ModeDisplay + "  ");
                    }
                    if(m_TX8mode != TX8mode){
                    lcd.setCursor(18,0);
                    lcd.print(rxtxmodeDisplay);
                    }

                    #ifdef CHANNEL_SET
                    if(chanelmodenabled == true){
                    lcd.setCursor(0,1);
                    lcd.print(chanBand +"       ");
                    }else{
                    
                    lcd.setCursor(0,1);
                    lcd.print( band_name[band_select]  + " BAND   ");
                    
                    }//end of chanelmodenabled if statement
                    #else
                    lcd.setCursor(0,1);
                    lcd.print( band_name[band_select] + " BAND   ");                   
                    #endif

                    if(m_tuning_select != gen_set_mem_array[tuning_select] || m_speed_select != x_speed_select ){
                    lcd.setCursor(0,2);
                    lcd.print(tuning_step_display + "   ");
                    }
                    #ifdef CHANNEL_SET
                    if(chanelmodenabled == true){
                    channelString = String(channel_num_array[chan_band_select]);
                    lcd.setCursor(0,3);
                    lcd.print("Channel " + channelString + " ");
                    }else{
                    lcd.setCursor(0,3);
                    lcd.print("               ");
                    }
                    #endif                    
                    #endif 
*/