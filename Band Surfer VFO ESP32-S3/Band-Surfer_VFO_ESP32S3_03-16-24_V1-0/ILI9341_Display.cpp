#include "ILI9341_Display.h"

Ucglib_ILI9341_18x240x320_HWSPI ucg(/*cd=*/ 46, /*cs=*/ 10, /*reset=*/ 45);
int mid_bar_y_pos = -8;
int mid_botom_y_pod = -10;

void ILI9341_DISPLAY::begin(void){
              ucg.begin(UCG_FONT_MODE_TRANSPARENT);
              ucg.setColor(1, 0, 0, 0);
              ucg.drawGradientBox(0, 0, ucg.getWidth(), ucg.getHeight());
}// end of begin function
//***********************************************************************
//***********************************************************************
void ILI9341_DISPLAY::modeDisplay(String display_mode){ //linked
          if(m_display_mode != display_mode){
              ucg.setRotate270();
              ucg.setFontMode(UCG_FONT_MODE_SOLID);
              ucg.setPrintPos(0,28);//35
              ucg.setFont(ucg_font_inr21_mr);
              ucg.setColor(0, 255, 255, 255);
              if(display_mode == "AM" || display_mode == "FM" || display_mode == "CW"){	
              ucg.print(display_mode+" ");}else{ucg.print(display_mode);}
              ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
              m_display_mode = display_mode;      
            }
}//end of modeDisplay function
//***********************************************************************
//***********************************************************************
void ILI9341_DISPLAY::vfoDisplay(uint8_t vfo_display){
        if(m_vfoArray != vfoArray[vfo_display]){
              ucg.setRotate270();
              ucg.setFontMode(UCG_FONT_MODE_SOLID);
              ucg.setPrintPos(66,28);
              ucg.setFont(ucg_font_inr21_mr); //ucg_font_inb24_mr
              ucg.setColor(0, 200, 200, 255);	
              ucg.print(vfoArray[vfo_display]);
              ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);

        if (vfo_display == 1){
              ucg.setRotate270();
              ucg.setFontMode(UCG_FONT_MODE_SOLID);
              ucg.setPrintPos(0,216+ mid_botom_y_pod);
              ucg.setFont(ucg_font_inr16_mr); //ucg_font_inb24_mr
              ucg.setColor(0, 200, 200, 255);	
              ucg.print(vfoArray[2]);
              ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
                }
         if (vfo_display == 2){
              ucg.setRotate270();
              ucg.setFontMode(UCG_FONT_MODE_SOLID);
              ucg.setPrintPos(0,216+ mid_botom_y_pod);
              ucg.setFont(ucg_font_inr16_mr); //ucg_font_inb24_mr
              ucg.setColor(0, 200, 200, 255);	
              ucg.print(vfoArray[1]);
              ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
                }
          m_vfoArray = vfoArray[vfo_display]; 
          }//end of if statement
}//end of vfoDisplay function
//***********************************************************************
//***********************************************************************

void ILI9341_DISPLAY::freq_incdisplay(bool oneHz, long displayFreqinc, String BandDisplay, bool chanelmodenabled){
uint8_t y_pos_1 = 88;
uint8_t y_pos_2 = 140;//140
bool m_chanelmodenabled;         

          if(displayFreqinc < 1e6){inc_select = 1;} 
          if(displayFreqinc >= 1e6){inc_select = 2;}
          if(m_inc_select != inc_select || m_BandDisplay != BandDisplay || m_oneHz != oneHz || m_chanelmodenabled != chanelmodenabled){              
              ucg.setRotate270();
              ucg.setFontMode(UCG_FONT_MODE_SOLID);
              ucg.setFont(ucg_font_inr16_mr); //ucg_font_inb24_mr
              ucg.setColor(0, 255, 255, 255);	
              if(displayFreqinc >= 100e6 || oneHz == true && displayFreqinc >= 10e6 || chanelmodenabled == true){
              ucg.setPrintPos(281,y_pos_2); //x,y
              ucg.print(freq_inc_name[inc_select]);
              ucg.setPrintPos(281+28,y_pos_1); //x,y
              ucg.print(freq_inc_name[0]);              
              }else{
              ucg.setPrintPos(281,y_pos_1); //x,y
              ucg.print(freq_inc_name[inc_select]); 
              ucg.setPrintPos(281,y_pos_2); //x,y
              ucg.print(freq_inc_name[0]);
              }
              ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);              
              m_BandDisplay = BandDisplay;
              m_inc_select = inc_select;
              m_oneHz = oneHz;
              m_chanelmodenabled = chanelmodenabled;              
            }
}// end of freq_incdisplay function
//***********************************************************************
//***********************************************************************

void ILI9341_DISPLAY::vfoRxtx(uint8_t vfoRxtx_display){
        if(!init_vfo_rxtx){
          ucg.setRotate270();
          ucg.setColor(255, 255, 255);  // Set color (0,R,G,B)
          ucg.drawRFrame(0, 160 + mid_bar_y_pos, 99, 30, 8);  // Start from top-left pixel (x,y,width,height,radius)
          init_vfo_rxtx = true;
        }
      if(m_vfo_rxtxArray != vfo_rxtx[vfoRxtx_display]){
          ucg.setRotate270();
          ucg.setFontMode(UCG_FONT_MODE_SOLID);
          ucg.setPrintPos(4,183 + mid_bar_y_pos);
          ucg.setFont(ucg_font_inr16_mr); //ucg_font_inb24_mr
          if(vfoRxtx_display == 1 || vfoRxtx_display == 3){ucg.setColor(0, 0, 0, 255);}else{
          ucg.setColor(0, 255, 255, 255);}
          ucg.print(vfo_rxtx[vfoRxtx_display]);
          ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
          m_vfo_rxtxArray = vfo_rxtx[vfoRxtx_display]; 
        }
}//end of vfo rxtx Display function

//***********************************************************************
//***********************************************************************

void ILI9341_DISPLAY::tuningstep(int tstep){
uint8_t x;     
      switch(tstep){case 0: x=119; break; case 1:x=114;break;case 2:x=107;break;case 3:x=114;break;}
      if(!init_tstep){
         ucg.setRotate270();
         ucg.setColor(255, 255, 255);  // Set color (0,R,G,B)
         ucg.drawRFrame(102, 161+ mid_bar_y_pos, 114, 30, 8);  // Start from top-left pixel (x,y,width,height,radius)
        init_tstep = true;
        }
      if(m_tstep != tstep){        
        ucg.setRotate270();
        ucg.setFontMode(UCG_FONT_MODE_SOLID);
        ucg.setPrintPos(107,183+ mid_bar_y_pos);
        ucg.setFont(ucg_font_inr16_mr); //ucg_font_inb24_mr
        ucg.setColor(0, 255, 255, 255);	
        ucg.print("        ");
        ucg.setPrintPos(x,183+ mid_bar_y_pos);
        ucg.print(tstepArray[tstep]);
        ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
         m_tstep = tstep; 
        }
}//end of tuning step function

//***********************************************************************
//***********************************************************************

void ILI9341_DISPLAY::tuningspeed(uint8_t tspeed){ //Linked
uint8_t x;        
        switch(tspeed){case 0: x=222;break;case 1: x=230;break; case 2:x=222;break;case 3: x=229;break;}        
        if(!init_tspeed){
          ucg.setRotate270();
          ucg.setColor(255, 255, 255);  // Set color (0,R,G,B)
          ucg.drawRFrame(219, 160+ mid_bar_y_pos, 99, 30, 8);  // Start from top-left pixel (x,y,wigth,height,radius)
          init_tspeed = true;
        }
       if(m_tspeed != tspeed){
          ucg.setRotate270();
          ucg.setFontMode(UCG_FONT_MODE_SOLID);
          ucg.setFont(ucg_font_inr16_mr); //ucg_font_inb24_mr
          if(tspeed == 3 ){ucg.setColor(0, 0, 0, 255);}else{
          ucg.setColor(0, 255, 255, 255);}
          ucg.setPrintPos(222,183+ mid_bar_y_pos);
          ucg.print("       ");
          ucg.setPrintPos(x,183+ mid_bar_y_pos);                       
          ucg.print(tspeedArray[tspeed]);
          ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
          m_tspeed = tspeed; 
        }
}//end of tuningspeed function

//***********************************************************************
//***********************************************************************

void ILI9341_DISPLAY::altvfofreq(long altvfofreq){

     if(m_altvfofreq != altvfofreq){
         altvfofreqMHz = (float) altvfofreq / 1e6;
         ucg.setRotate270();
         ucg.setFontMode(UCG_FONT_MODE_SOLID);
         ucg.setPrintPos(58,216 + mid_botom_y_pod);
         ucg.setFont(ucg_font_inr16_mr); //ucg_font_inb24_mr
         ucg.setColor(0, 200, 200, 255);	
         ucg.print(altvfofreqMHz, 5);
         ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
         m_altvfofreq = altvfofreq;
       }
}// end of altvfofreq

//***********************************************************************
//***********************************************************************

void ILI9341_DISPLAY::band_display(bool chanelmodenabled, String band_name){
String band;
if(chanelmodenabled == true){band = "     ";}else{band = " BAND   ";}

      if (m_band_name != band_name){
         ucg.setRotate270();
         ucg.setFontMode(UCG_FONT_MODE_SOLID);
         ucg.setPrintPos(200,216 + mid_botom_y_pod);
         ucg.setFont(ucg_font_inr16_mr); //ucg_font_inb24_mr
         ucg.setColor(0, 255, 255, 255);	
         ucg.print(band_name + band);
         ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);         
         m_band_name = band_name;
      }
}// end of band_display function

//***********************************************************************
//***********************************************************************

void ILI9341_DISPLAY::utcTime(String timeUTC){
uint8_t xpos = 27;  
      if(!init_timeUTC){
          ucg.setRotate270();
          ucg.setFontMode(UCG_FONT_MODE_SOLID);
          ucg.setPrintPos(196,xpos);
          ucg.setFont(ucg_font_fur14_hf); //ucg_font_inb24_mr
          ucg.setColor(0, 255, 255, 255);	
          ucg.print("UTC");
          ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
          init_timeUTC = true;    
       }

      if(m_timeUTC != timeUTC){
          ucg.setRotate270();
          ucg.setFontMode(UCG_FONT_MODE_SOLID);
          ucg.setPrintPos(145,xpos);
          ucg.setFont(ucg_font_fur14_hf); //ucg_font_inb24_mr
          ucg.setColor(0, 255, 255, 255);	
          ucg.print(timeUTC);
          ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
          m_timeUTC = timeUTC;
        }        
}//end of utcTime function

//***********************************************************************
//***********************************************************************

void ILI9341_DISPLAY:: rxtxDisplay(bool txMode){ //linked
        if(!init_txMode){
          ucg.setRotate270();
          ucg.setColor(255, 255, 255);  // Set color (0,R,G,B)
          ucg.drawRFrame(247, 0, 70, 32, 7);  // Start from top-left pixel (x,y,wigth,height,radius) //ucg.drawRFrame(247, 0, 70, 41, 12);  // Start from top-left pixel (x,y,wigth,height,radius)
          init_txMode = true;
        }

        if(m_txMode != txMode){
        if(txMode == true){
          ucg.setRotate270();
          ucg.setColor(255, 0, 0);  // Set color (0,R,G,B)
          ucg.drawRBox(250, 3, 64, 26, 5);  // Start from top-left pixel (x,y,wigth,height,radius) // solid object  
          ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
          ucg.setPrintPos(265,26);
          ucg.setFont(ucg_font_inb21_mr);
          ucg.setColor(0, 255, 255, 255);	
          ucg.print("TX");
          ucg.setFontMode(UCG_FONT_MODE_SOLID);
          }else{
          ucg.setRotate270();
          ucg.setColor(0, 255, 0);  // Set color (0,R,G,B)
          ucg.drawRBox(250, 3, 64, 26, 5);  // Start from top-left pixel (x,y,wigth,height,radius) // solid object  
          ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
          ucg.setPrintPos(265,26);
          ucg.setFont(ucg_font_inb21_mr);
          ucg.setColor(0, 0, 0, 0);	
          ucg.print("RX");
          ucg.setFontMode(UCG_FONT_MODE_SOLID);
          }


          m_txMode = txMode;
          }//end of main if statement
}//end of rxtxDisplay function

//***********************************************************************
//***********************************************************************

void ILI9341_DISPLAY::signal_bargraph(bool tx, int siglev, bool sql_enable, int sql_lev, bool sql_active){
uint8_t bar_width = 6;
uint8_t bar_hieght = 12;
uint8_t bar_xpos = 137;
int bar_peak_lev = 0;
m_sql_enable = sql_enable;
m_tx = tx;
m_sql_active = sql_active;
tx8 = tx;


          if(tx8 == false && !m_rx_constructor){ //RX
            ucg.setRotate270();
            ucg.setColor(0, 0, 0, 0);	
            ucg.drawBox( 0, 106 + rx_bar_x_pos, 281, 24); // draw bar (y pos, x pos, bar_width, bar_hieght) // white bar
            ucg.setColor(0, 255, 255, 255);	
            ucg.drawBox( 0, 130+rx_bar_x_pos, 172, 3); // draw bar (y pos, x pos, bar_width, bar_hieght) // white bar 
            ucg.setColor(0, 255, 0, 0);	
            ucg.drawBox( 173, 130+rx_bar_x_pos, 102, 3); // red bar
            ucg.setColor(0, 255, 255, 255);	 //ucg.drawBox( 0, 126, 3, 8);
            ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
            ucg.setPrintPos(0,118+rx_bar_x_pos);
            ucg.setFont(ucg_font_profont17_mr);
            ucg.setColor(0, 255, 255, 255);	
            ucg.print("S");
            ucg.setFont(ucg_font_profont15_mr);
            ucg.setPrintPos(32,118+rx_bar_x_pos);
            ucg.print("1");
            ucg.setPrintPos(32+34,118+rx_bar_x_pos);
            ucg.print("3");
            ucg.setPrintPos(66+34,118+rx_bar_x_pos);
            ucg.print("5");
            ucg.setPrintPos(101+34,118+rx_bar_x_pos);
            ucg.print("7");
            ucg.setPrintPos(135+34,118+rx_bar_x_pos);
            ucg.print("9");
            ucg.setPrintPos(159+34,118+rx_bar_x_pos);
            ucg.print("+20");
            ucg.setPrintPos(192+34,118+rx_bar_x_pos);
            ucg.print("+40");
            ucg.setPrintPos(227+34,118+rx_bar_x_pos);
            ucg.print("+60");
            ucg.setFontMode(UCG_FONT_MODE_SOLID);
            longstab = true;
                for (int i = 0; i <= 280; ) {
                if(longstab == true){
                ucg.drawBox( i, 120+rx_bar_x_pos, 2, 10); // draw bar (y pos, x pos, bar_width, bar_hieght)
                }
                if(longstab == false){
                ucg.drawBox( i, 126+rx_bar_x_pos, 2, 4); // draw bar (y pos, x pos, bar_width, bar_hieght)
                }
                i+=17;//17
                longstab =! longstab;            
                }
            ucg.setColor(0, 0, 255, 0); 
            m_rx_constructor = true;  
            m_tx_constructor = false;
          }// end of if statement


          if(tx8 == true && !m_tx_constructor){ //TX
            ucg.setRotate270();
            ucg.setColor(0, 0, 0, 0);	
            ucg.drawBox( 0, 106 + rx_bar_x_pos, 281, 24); // draw bar (y pos, x pos, bar_width, bar_hieght) // white bar
            ucg.setColor(0, 255, 0, 0);	// white bar
            ucg.drawBox( 0, 130+rx_bar_x_pos, 272, 3); // draw bar (y pos, x pos, bar_width, bar_hieght) // white bar
            ucg.setColor(0, 0, 0, 0);	// white bar
            ucg.drawBox( 273, 130+rx_bar_x_pos, 2, 3); // draw bar (y pos, x pos, bar_width, bar_hieght) // white bar 
            ucg.setColor(0, 255, 255, 255);	 //ucg.drawBox( 0, 126, 3, 8);
            ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);  
            ucg.setFont(ucg_font_profont17_mr);
            ucg.setPrintPos(0,117+rx_bar_x_pos);
            ucg.setColor(0, 255, 255, 255);	
            ucg.print("W");
            ucg.setFont(ucg_font_profont15_mr);
            ucg.setPrintPos(25,122+rx_bar_x_pos);
            ucg.print("1");
            ucg.setPrintPos(52,122+rx_bar_x_pos);
            ucg.print("2");
            ucg.setPrintPos(79,122+rx_bar_x_pos);
            ucg.print("3");
            ucg.setPrintPos(106,122+rx_bar_x_pos);
            ucg.print("4");
            ucg.setPrintPos(134,122+rx_bar_x_pos);
            ucg.print("5");
            ucg.setPrintPos(161,122+rx_bar_x_pos);
            ucg.print("6");
            ucg.setPrintPos(188,122+rx_bar_x_pos);
            ucg.print("7");
            ucg.setPrintPos(215,122+rx_bar_x_pos);
            ucg.print("8");
            ucg.setPrintPos(242,122+rx_bar_x_pos);
            ucg.print("9");
            ucg.setPrintPos(266,117+rx_bar_x_pos);
            ucg.print("10");
            ucg.setFontMode(UCG_FONT_MODE_SOLID);

          for (int i = 0; i <= 280; ) {            
              ucg.setColor(0, 255, 255, 255);
              if(i==0 || i ==270){ 
              ucg.drawBox( i, 120+rx_bar_x_pos, 2, 10); // draw bar (y pos, x pos, bar_width, bar_hieght)             
              }else{
              ucg.drawBox( i, 126+rx_bar_x_pos, 2, 4); // draw bar (y pos, x pos, bar_width, bar_hieght)
              }
              i+=27;//17                         
              }
              ucg.setColor(0, 0, 255, 0);	
              m_rx_constructor = false;
              m_tx_constructor = true;
            }//end of if statement        

         if(millis() - m_millis > 80){// bar meter update timing
            m_siglev = siglev / 14;
            m_millis = millis();
            }// end of millis if statement

        if(m_siglev>270){m_siglev = 270;}//else{m_siglev = m_siglev;} // signal level limitor 

          for (int i = 3; i <= m_siglev; i++){
          if(tx8 == true){ucg.setColor(0, 255, 255, 255);}else{
          if(m_sql_active == true){ucg.setColor(0, 189, 189, 189); }else{ 
          if (i < 170){ucg.setColor(0, 0, 255, 0);}else{ ucg.setColor(0, 255, 0, 0);} } }
          ucg.drawBox( i -1, bar_xpos+rx_bar_x_pos, bar_width, bar_hieght ); // draw bar (y pos, x pos, bar_width, bar_hieght)
          bar_peak_lev = i;
          i+=8;
          }// end of for loop
          
          bar_peak_lev += bar_width;

          int peak_lim = 272;

          if(bar_peak_lev >= peak_lim || peak_marker >= peak_lim  ){bar_peak_lev = peak_lim; peak_marker = peak_lim;} // marker limitor                

          if(bar_peak_lev >= peak_marker ){
          ucg.setColor(0, 0, 0, 0);  
          ucg.drawBox(prev_peak_marker, bar_xpos+rx_bar_x_pos, 2, bar_hieght ); 
          if(tx8 == true){ucg.setColor(0, 255, 0, 0);}else{
          ucg.setColor(0, 255, 255, 255);}
          peak_marker = bar_peak_lev;
          ucg.drawBox(peak_marker, bar_xpos+rx_bar_x_pos, 2, bar_hieght ); 
          prev_peak_marker = peak_marker;
          peak_hold = true;
          mm_millis = millis();
          }

          int blanker_width = peak_marker - bar_peak_lev;
          if(bar_peak_lev + blanker_width >= 273 ){blanker_width = 273 - bar_peak_lev;}// blanker limitor
          ucg.setColor(0, 0, 0, 0);
          ucg.drawBox( bar_peak_lev, bar_xpos+rx_bar_x_pos, blanker_width, bar_hieght ); // draw bar (y pos, x pos, bar_width, bar_hieght)
 
          if(millis() - mm_millis > 3000 && peak_hold == true){
          peak_marker = bar_peak_lev;
          peak_hold = false;
          }
     
          if(m_sql_enable == true){
          sql_lev = sql_lev / 14;
          if(sql_lev >= 260){sql_lev = 260;}
          if(m_sql_lev != sql_lev ){          
            ucg.setRotate270();
            ucg.setColor(0, 224,224, 224);	
            ucg.drawBox( 3, 153+rx_bar_x_pos, sql_lev, 4); // squelch bar // draw bar (y pos, x pos, bar_width, bar_hieght)
            ucg.setColor(0, 0, 0, 0);	
            ucg.drawBox( sql_lev, 153+rx_bar_x_pos, 260 - sql_lev, 4); // draw bar (y pos, x pos, bar_width, bar_hieght)           
            m_sql_lev = sql_lev;
           }
          }

}//end of signal_bargraph function
//*************************************************************************
//*************************************************************************
long ILI9341_DISPLAY::FreqDisplay(bool chanelmodenabled, bool oneHz, long displayFreq){

      if(m_chanelmodenabled != chanelmodenabled){m_chanelmodenabled = chanelmodenabled;}
      if(displayFreq >= 100e6 || m_chanelmodenabled == true){oneHz = false;}// limits the 1 Hz dispaly at 100Mhz of higher or in channel mode

      if(m_chanelmodenabled == true && clearedFreqdisplay == false){ 
      ucg.setRotate270();
      ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
      ucg.setFontMode(UCG_FONT_MODE_SOLID);
      ucg.setFont(XFX8);   // ucg_font_inr46_mn // ucg_font_logisoso54_tn); //ucg_font_logisoso46_tn ///ucg_font_inb53_mn //DSEG14_ClassicTEST //ucg_font_inr46_mn
      ucg.setColor(0, 255, 255, 255);		// use white as main color for the font
      ucg.setColor(1, 0, 0, 0);			// use blue as background for SOLID mode
      ucg.setPrintPos(0, y_freq_dis_pos);//100 // 94
      clearedFreqdisplay = true;                 
      }// end of clearFreqdisplay if statement      
      uint8_t  x_pos_1;
      uint8_t  x_pos_2;
      uint8_t  x_pos_3;

      ucg.setRotate270();
      ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
      ucg.setFontMode(UCG_FONT_MODE_SOLID);
      ucg.setFont(XFX8);   // ucg_font_inr46_mn // ucg_font_logisoso54_tn); //ucg_font_logisoso46_tn ///ucg_font_inb53_mn //DSEG14_ClassicTEST //ucg_font_inr46_mn
      ucg.setColor(0, 255, 255, 255);		// use white as main color for the font
      ucg.setColor(1, 0, 0, 0);			// use blue as background for SOLID mode
      ucg.setPrintPos(0, y_freq_dis_pos);//100 // 94

      m_displayFreq = displayFreq;
      int x_pos = 0;
      int y_pos_inc = 0;
      int digits_num;
      int digit_dis;      
      int testcounter;
      int lastdigit;

      if(displayFreq < 1e6 ){testcounter=6;  digits_num = 5; digit_dis = 7; lastdigit = 6;}  
      if(displayFreq < 10e6 && displayFreq >= 1e6){testcounter=8;  digits_num = 6; digit_dis = 9; lastdigit = 8;}
      if(displayFreq >= 10e6 && displayFreq < 100e6){testcounter=9;  digits_num = 7; digit_dis = 10; lastdigit = 9;}
      if(displayFreq >= 100e6){testcounter=10; digits_num = 8; digit_dis = 11; lastdigit = 10;}

              for (int i = digits_num; i >= 0; i--) { //7
                  displayFreqarray[i] = m_displayFreq % 10;
                  m_displayFreq /= 10;
              }// end of for loop

       int j =0;

              if(chanelmodenabled == true){
              for (int i = 0; i < digit_dis; i++) {
                      x_pos = 96;
                      y_pos_inc = -12;
                      if(displayFreq < 1e6 ){x_pos_1 = x_pos_1 = 11; x_pos_2 = 27; x_pos_3 = 27; switch(i){case 3:xdisplayFreqarray[i] = char(44);i +=1; break;}}
                      if(displayFreq < 10e6 && displayFreq >= 1e6){x_pos_1 = 11; x_pos_2 = 27; x_pos_3 = 27; switch(i){case 1:xdisplayFreqarray[i] = char(44);i +=1; break;case 5:xdisplayFreqarray[i] = char(44);i +=1;break;}}
                      if(displayFreq >= 10e6 && displayFreq < 100e6){x_pos_1 = 11; x_pos_2 = 27; x_pos_3 = 27; switch(i){case 2:xdisplayFreqarray[i] = char(44);i +=1; break;case 6:xdisplayFreqarray[i] = char(44);i +=1;break;}}
                      if(displayFreq >= 100e6){x_pos_1 = 11; x_pos_2 = 27; x_pos_3 = 27; switch(i){case 3:xdisplayFreqarray[i] = char(44);i +=1; break;case 7:xdisplayFreqarray[i] = char(44);i +=1;break;}} 
                      xdisplayFreqarray[i] = String(displayFreqarray[j]);
                      j++;                      
                   }// end of for loop   
              }else{
              for (int i = 0; i < digit_dis; i++) { 
                      x_pos = 0;
                      y_pos_inc = 0;
                      if(displayFreq < 1e6 ){x_pos_1 = 16; x_pos_2 = 38; x_pos_3 = 36; switch(i){case 3:xdisplayFreqarray[i] = char(44);i +=1; break;}}
                      if(displayFreq < 10e6 && displayFreq >= 1e6){x_pos_1 = 14; x_pos_2 = 36; x_pos_3 = 36; switch(i){case 1:xdisplayFreqarray[i] = char(44);i +=1; break;case 5:xdisplayFreqarray[i] = char(44);i +=1;break;}}
                      if(displayFreq >= 10e6 && displayFreq < 100e6){x_pos_1 = 12; x_pos_2 = 35; x_pos_3 = 36; switch(i){case 2:xdisplayFreqarray[i] = char(44);i +=1; break;case 6:xdisplayFreqarray[i] = char(44);i +=1;break;}} 
                      if(displayFreq >= 100e6){x_pos_1 = 12; x_pos_2 = 35; x_pos_3 = 36; switch(i){case 3:xdisplayFreqarray[i] = char(44);i +=1; break;case 7:xdisplayFreqarray[i] = char(44);i +=1;break;}} 
                      xdisplayFreqarray[i] = String(displayFreqarray[j]);
                      j++;
                   }// end of for loop
              }//end of chanelmodenabled if

              
                    if(mmm_chanelmodenabled != chanelmodenabled){
                      ucg.setFontMode(UCG_FONT_MODE_SOLID);
                      ucg.print(blanker);
                      mmm_chanelmodenabled = chanelmodenabled;
                      fullfreqscan = true;
                     }

                    if(displayFreq < 1e6 && clearlcdfreq0 == true){
                      ucg.setFontMode(UCG_FONT_MODE_SOLID);
                      ucg.print(blanker);  
                      clearlcdfreq0 =! clearlcdfreq0;
                      clearlcdfreq1 = true;
                      clearlcdfreq2 = true;
                      clearlcdfreq3 = true;
                      fullfreqscan = true;
                      }

                      if(displayFreq < 10e6 && displayFreq >= 1e6  && clearlcdfreq1 == true){
                      ucg.setFontMode(UCG_FONT_MODE_SOLID);
                      ucg.print(blanker);
                      clearlcdfreq1 =! clearlcdfreq1;
                      clearlcdfreq0 = true;
                      clearlcdfreq2 = true;
                      clearlcdfreq3 = true;
                      fullfreqscan = true;                          
                     }

                    if(displayFreq >= 10e6 && displayFreq < 100e6 && clearlcdfreq2 == true){
                      ucg.setFontMode(UCG_FONT_MODE_SOLID);
                      ucg.print(blanker);
                      clearlcdfreq2 =! clearlcdfreq2;
                      clearlcdfreq0 = true;
                      clearlcdfreq1 = true;
                      clearlcdfreq3 = true; 
                      fullfreqscan = true;             
                     }

                    if(displayFreq >= 100e6 && displayFreq && clearlcdfreq3 == true){
                      ucg.setFontMode(UCG_FONT_MODE_SOLID);
                      ucg.print(blanker);
                      clearlcdfreq3 =! clearlcdfreq3;
                      clearlcdfreq0 = true;
                      clearlcdfreq1 = true;
                      clearlcdfreq2 = true; 
                      fullfreqscan = true;          
                     } 

                    if(oneHz == false){
                    ucg.setFont(XFX8); 
                    xdisplayFreqarray[lastdigit] = char(46);                                     
                    }



                    if(chanelmodenabled == true){ ucg.setFont(XFX1);}else{ ucg.setFont(XFX8);}

                    for (int i = 0; i <= testcounter; i++) { //10    8= 1-10
                        ucg.setPrintPos(x_pos,y_freq_dis_pos + y_pos_inc);//100 //94
                        if(xdisplayFreqarray[i] != xm_displayFreqarray[i] || fullfreqscan == true){
                        ucg.print(xdisplayFreqarray[i]);
                        ucg.setPrintPos(x_pos, y_freq_dis_pos + y_pos_inc);
                        xm_displayFreqarray[i] = xdisplayFreqarray[i];
                        }

                    if(displayFreq < 1e6){switch(i){case 2:ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);x_pos += x_pos_2;break;case 3:ucg.setFontMode(UCG_FONT_MODE_SOLID);x_pos += x_pos_1;break;
                    case 6:ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);x_pos += x_pos_2;break;case 7:ucg.setFontMode(UCG_FONT_MODE_SOLID);x_pos += x_pos_1;break;default:x_pos += x_pos_3;}}

                    if(displayFreq < 10e6 && displayFreq >= 1e6){switch(i){case 0:ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);x_pos += x_pos_2;break;case 1:ucg.setFontMode(UCG_FONT_MODE_SOLID);x_pos += x_pos_1;break;
                    case 4:ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);x_pos += x_pos_2;break;case 5:ucg.setFontMode(UCG_FONT_MODE_SOLID);x_pos += x_pos_1;break;default:x_pos += x_pos_3;}}

                    if(displayFreq >= 10e6 && displayFreq < 100e6){switch(i){case 1:ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);x_pos += x_pos_2;break;case 2:ucg.setFontMode(UCG_FONT_MODE_SOLID);x_pos += x_pos_1;break;
                    case 5:ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);x_pos += x_pos_2;break;case 6:ucg.setFontMode(UCG_FONT_MODE_SOLID);x_pos += x_pos_1;break;default:x_pos += x_pos_3;}} 
                    if(displayFreq >= 100e6){switch(i){case 2:ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);x_pos += x_pos_2;break;case 3:ucg.setFontMode(UCG_FONT_MODE_SOLID);x_pos += x_pos_1;break;
                    case 6:ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);x_pos += x_pos_2;break;case 7:ucg.setFontMode(UCG_FONT_MODE_SOLID);x_pos += x_pos_1;break;default:x_pos += x_pos_3;}}                                 
                    }// end of for loop
                    fullfreqscan = false;
                    clearedFreqdisplay = false;
return displayFreq;
}// end of FreqDisplay function
//*************************************************************************
//*************************************************************************
void ILI9341_DISPLAY::ChannelDisplay(bool chanelmodenabled, uint8_t channelNum){

                if(chanelmodenabled == true){
                  if(m_channelNum != channelNum){  
                  String chanstring;
                  chanstring = String(channelNum);
                  ucg.setRotate270();
                  ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
                  ucg.setFontMode(UCG_FONT_MODE_SOLID);
                  ucg.setFont(XFX8);   // ucg_font_inr46_mn // ucg_font_logisoso54_tn); //ucg_font_logisoso46_tn ///ucg_font_inb53_mn //DSEG14_ClassicTEST //ucg_font_inr46_mn
                  ucg.setColor(0, 255, 0, 0);		// use white as main color for the font
                  ucg.setColor(1, 0, 0, 0);			// use blue as background for SOLID mode
                  ucg.setPrintPos(0, y_freq_dis_pos);//100 // 94
                  if(channelNum < 10){                  
                  ucg.print("0"+chanstring);
                  }else{
                  ucg.print(chanstring);
                  }
                  m_channelNum = channelNum;
                }//end of channelNum if statement
                }else{m_channelNum = 0;}//end of chanelmodenabled if statement

}//end of ChannelFreqDisplay function
//*************************************************************************
//*************************************************************************
