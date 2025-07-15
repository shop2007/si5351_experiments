#include "MAX7219_Display.h"



void MAX7219_X::begin(uint8_t LED_brightness){
  lc=LedController<1,1>(DIN,CLK,CS);
  lc.setIntensity(LED_brightness);  
  lc.clearMatrix();
}//end of begin function


void MAX7219_X::MAX7219_display(long Main_Display_FreqHz, long Alt_Display_FreqH, bool vfosplitenabled){
  

          if(m_Main_Display_FreqHz != Main_Display_FreqHz){
          switch(Main_Display_FreqHz){
                    case 1000 ... 999999: //KHz
                      point_marker [2] = true;                                                      
                      break;
                    case 1000000 ... 1000000000: //MHz
                      point_marker [5] = true;                                       
                      break;
              }

        if(Main_Display_FreqHz < freq_rangeHz[0]){
          if(freq_range[0] == true){ 
            lc.clearMatrix(); freq_range[0] =! freq_range[0];} 
            }else{freq_range[0] = true;}

        if(Main_Display_FreqHz < freq_rangeHz[1]){
          if(freq_range[1] == true){ 
            lc.clearMatrix(); freq_range[1] =! freq_range[1];} 
            }else{freq_range[1] = true;} 

        if(Main_Display_FreqHz < freq_rangeHz[2]){
          if(freq_range[2] == true){ 
            lc.clearMatrix(); freq_range[2] =! freq_range[2];} 
            }else{freq_range[2] = true;}
int j = -1;
m_freqHz = Main_Display_FreqHz;
              while (m_freqHz > 0){
                   int digit = m_freqHz %10;
                   m_freqHz  /= 10;                    
                   lc.setChar(0,j,digit,point_marker [j]);                   
                   j++;
                }

          m_Main_Display_FreqHz = Main_Display_FreqHz;
          }//end of main if statement



}//end of MAX7219_display function