#include "SSD1306_Display.h"

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

const uint8_t XFX2[940] U8G2_FONT_SECTION("XFX2") = 
  "\12\2\6\5\6\6\1\2\7'\60\5\376+\0+\0\0\0\0\0\3\217\60\230'\234\63r=Y\23"
  "\4W\63\4\225 (D\60\11\203R\4\221@(\206T(Fd(Fd(Fd(Fd(F"
  "d(Fd(Fd(Fd(Fd(Fd(Fd(Fd(Fd(\206T(\5E("
  "\204\65(\3&(\3IX\203RP\204bH\205bD\206bD\206bD\206bD\206bD\206b"
  "D\206bD\206bD\206bD\206bD\206bD\206bD\206bD\206bH\205R\4\221@(D"
  "\60\11\203\62\4\225 ,Ap=\331s!\0\61('\234\363\17\272\4r\30\11\261\10\315\377\23\213"
  "\221 \207R\370\1f\30\11\261\10\315\377\23\213\221 \207R\370\201\35\0\62@'\234\63r=\331s"
  "E\225 <\223\60@\221@\210Eh\376\237X@\221@<\223\60\70\225 (CP\71D\60=E"
  "\20AFh\376\277\260\10\42\210\10\246g\10\252'\10\256'{.\4\0\63>'\234\63r=\331s"
  "E\225 <\223\60@\221@\210Eh\376\237X@\221@<\223\60\70\225 \70\325\64\11\3\24\11\204"
  "X\204\346\377\211\5\24\11\304\63\11\203S\11Bs=\331s!\0\64a'\234\363\17TA\71`\202"
  "BX\203RP\204bH\205bD\206bD\206bD\206bD\206bD\206bD\206bD\206bD"
  "\206bD\206bD\206bD\206bD\206bD\206bH\205R\4\221@(D\60\11\203\62\4\225 "
  "\70\325\64\11\3\24\11\204X\204\346\377\211\305H\220C)\374\300\16\0\65>'\234\63r=Y\23\4"
  "W\63\4\225C\4\323S\4\21d\204\346\377\13\213 \202\210`z\206\240\222\252i\22\6(\22\10\261"
  "\10\315\377\23\13(\22\210g\22\6\247\22\204\346z\262\347B\0\66p'\234\63r=Y\23\4W\63"
  "\4\225C\4\323S\4\21d\204\346\377\13\213 \202\210`z\206\240r\206\240r\210`\22\6\245\10\42"
  "\201P\14\251P\214\310P\214\310P\214\310P\214\310P\214\310P\214\310P\214\310P\214\310P\214\310P"
  "\214\310P\214\310P\214\310P\214\310P\14\251P\212 \22\10\205\10&aP\206\240\22\204%\10\256'"
  "{.\4\0\67/'\234\63r=\331sE\225 <\223\60@\221@\210Eh\376\237X\214\4\71\224"
  "\302\17\60\303H\210Eh\376\237X\214\4\71\224\302\17\354\0\70\241'\234\63r=Y\23\4W\63\4"
  "\225 (D\60\11\203R\4\221@(\206T(Fd(Fd(Fd(Fd(Fd(Fd"
  "(Fd(Fd(Fd(Fd(Fd(Fd(Fd(\206T(E\20\11\204B\4\223"
  "\60(CP\11\202\62\4\225C\4\223\60(E\20\11\204bH\205bD\206bD\206bD\206bD"
  "\206bD\206bD\206bD\206bD\206bD\206bD\206bD\206bD\206bD\206bH\205R"
  "\4\221@(D\60\11\203\62\4\225 ,Ap=\331s!\0\71p'\234\63r=Y\23\4W\63"
  "\4\225 (D\60\11\203R\4\221@(\206T(Fd(Fd(Fd(Fd(Fd(F"
  "d(Fd(Fd(Fd(Fd(Fd(Fd(Fd(\206T(E\20\11\204B\4"
  "\223\60(CP\11\202SM\223\60@\221@\210Eh\376\237X@\221@<\223\60\70\225 \64\327\223"
  "=\27\2\0\0\0\0\4\377\377\0";


void SSD1306_DISPLAY::begin(){  
u8g2.setI2CAddress(0x78); // I2C address for OLED
u8g2.begin();
}//end of begin funtion

void SSD1306_DISPLAY::display_freq_res(uint8_t oneHzRes, bool oneHz){
          if(m_oneHzRes != oneHzRes || m_oneHz != oneHz){  
          m_oneHzRes = oneHzRes;
          m_oneHz = oneHz;
          update_freq_dispaly = true;
          }// end of if
}//end of display_freq_res function
//***********************************************************************
//***********************************************************************
void SSD1306_DISPLAY::dispaly_freq(long Alt_Display_FreqHz, long Main_Display_FreqHz, bool TX8mode, bool vfosplitenabled, bool chanelmodenabled){
          long OLED_freq_displayHz;          

          if(vfosplitenabled == true && TX8mode == true){OLED_freq_displayHz = Alt_Display_FreqHz;
          }else{OLED_freq_displayHz = Main_Display_FreqHz;}
          if(OLED_freq_displayHz < 1e6){
          displayFreqMHz = (float) OLED_freq_displayHz / 1e3;  //1e6;
          freq_inc_select=0;
          decimal_places = 3 + m_oneHzRes;
          }else{
          displayFreqMHz = (float) OLED_freq_displayHz / 1e6;
          freq_inc_select = 1;
          decimal_places = 5 + m_oneHzRes;
          }
          if(m_displayFreqMHz != displayFreqMHz || update_freq_dispaly == true || m_vfosplitenabled != vfosplitenabled){          
          m_displayFreqMHz = displayFreqMHz;
          m_vfosplitenabled = vfosplitenabled;
          update_freq_dispaly = false;
          VFO_OLED_SSD1306();
          }
}// end of function
//***********************************************************************
//***********************************************************************
void SSD1306_DISPLAY::display_tstep_speed(String tuning_step, String tuning_speed){
          tuning_step_display = "TS " + tuning_step + tuning_speed;
          if(m_tuning_step_display != tuning_step_display){          
          m_tuning_step_display = tuning_step_display;
          VFO_OLED_SSD1306();  
          }
}//end of function
//***********************************************************************
//***********************************************************************

void SSD1306_DISPLAY::display_band_mode(String BandDisplay, String ModeDisplay, bool TX8mode, bool modeDisplayenable, bool rxtxDisplayenable){

if(TX8mode == true){rxtxmodeDisplay = "TX";}else{rxtxmodeDisplay = "RX";}
m_BandDisplay = BandDisplay;

if(modeDisplayenable == true){band_display =  BandDisplay  + " " + ModeDisplay;}else{band_display =  BandDisplay  + " BAND";}   

if(m_band_display != band_display || m_modeDisplayenable != modeDisplayenable || m_TX8mode != TX8mode || m_rxtxDisplayenable != rxtxDisplayenable){

m_band_display = band_display;
m_modeDisplayenable = modeDisplayenable;
m_TX8mode = TX8mode; 
m_rxtxDisplayenable = rxtxDisplayenable;
update_freq_dispaly = true;
VFO_OLED_SSD1306();
}
}//end of function

//***********************************************************************
//***********************************************************************
void SSD1306_DISPLAY::display_vfo(uint8_t vfo_Display){
test = vfo_Display;

if(m_vfo_Display != vfo_Display)
m_vfo_Display = vfo_Display;
VFO_OLED_SSD1306();
}//end of function
//***********************************************************************
//***********************************************************************
void SSD1306_DISPLAY::display_channel_mode(bool chanelmodenabled, int vfo_A_chan_num, int vfo_B_chan_num){

m_chanelmodenabled = chanelmodenabled;
//#define vfo_A 1 
//#define vfo_B 2 

if(m_vfo_Display = 1){display_chan = vfo_A_chan_num;}else{display_chan = vfo_B_chan_num;}
//if(m_vfo_Display = vfo_B){display_chan = vfo_B_chan_num;}

if(m_display_chan != display_chan){
m_display_chan = display_chan;
VFO_OLED_SSD1306();
}


}//end of function

void SSD1306_DISPLAY::VFO_OLED_SSD1306(){

  Serial.println("VFO_OLED_SSD1306");

  if(m_chanelmodenabled == true){

  String channel = (String) display_chan;
  String chan_freq = String (displayFreqMHz,5);
  String Split;

  if(m_vfosplitenabled == true){Split ="S";}else{Split =" ";}

                      u8g2.firstPage();
                  do {
                      u8g2.setFont(u8g2_font_10x20_mr);
                      u8g2.setCursor(0, 13);
                      u8g2.print (m_BandDisplay +" "+ chan_freq);
                      u8g2.setCursor(100, 10);
                      u8g2.setCursor(0, 60);
                      u8g2.print (vfo[test]);
                      u8g2.setCursor(118, 60);
                      u8g2.print (Split);                  
                      u8g2.setFont(XFX2);
                      u8g2.setCursor(21, 62); //40 
                      if(display_chan < 10){
                      u8g2.print ("0" + channel);
                      }else{
                      u8g2.print (channel);
                      }

                      } while ( u8g2.nextPage() );



























  }else{

                  u8g2.firstPage();
                  do {
                      u8g2.setFont(u8g2_font_smart_patrol_nbp_tf);
                      u8g2.setCursor(0, 10);
                      u8g2.print (band_display);
                      u8g2.setCursor(81, 10);
                      if(m_vfosplitenabled == true){u8g2.print (vfo[m_vfo_Display]+"::");}else{
                      u8g2.print (vfo[m_vfo_Display]);
                      }
                      u8g2.setCursor(100, 10);                      
                      if(m_rxtxDisplayenable == true){
                      u8g2.print (rxtxmodeDisplay); //freq_inc_name[freq_inc_select] //rxtxmodeDisplay //ModeDisplay
                      }else{
                      u8g2.print(freq_inc_name[freq_inc_select]);
                      }                      
                      u8g2.setCursor(0, 64);
                      u8g2.print (tuning_step_display); //tuning_step_name                    
                      u8g2.setFont(u8g2_font_logisoso24_tn);
                      u8g2.setCursor(0, 45); //40 
                      u8g2.print (displayFreqMHz, decimal_places);
                      } while ( u8g2.nextPage() ); 
  } //end of if










  
}//end of function





















/*

void SSD1306_DISPLAY::Display(String band_display, bool rxtxDisplayenable, String rxtxmodeDisplay, String freq_inc_name, String tuning_step_display, float displayFreqMHz, uint8_t decimal_places, bool chanelmodenabled, int channel, uint8_t chan_band_select,uint8_t OLED_VFO, bool vfosplitenabled){
  
channelNum = String(channel); 
ChannelFreq = String(displayFreqMHz, 5);

if(vfosplitenabled == true && chanelmodenabled == true){Split ="S";}else{Split =" ";if(vfosplitenabled == true && chanelmodenabled == false){Split =":";}else{Split =" ";}}


                 
                if(chanelmodenabled == true){
                      u8g2.firstPage();
                  do {
                      u8g2.setFont(u8g2_font_10x20_mr);
                      u8g2.setCursor(0, 13);
                      u8g2.print (ChannelBand[chan_band_select] + " " + ChannelFreq );
                      u8g2.setCursor(100, 10);
                      u8g2.setCursor(0, 60);
                      u8g2.print (vfo[OLED_VFO]);
                      u8g2.setCursor(118, 60);
                      u8g2.print (Split);                  
                      u8g2.setFont(XFX2);
                      u8g2.setCursor(21, 62); //40 
                      if(channel < 10){
                      u8g2.print ("0" + channelNum);
                      }else{
                      u8g2.print (channelNum);
                      }

                      } while ( u8g2.nextPage() );

                }else{
                  u8g2.firstPage();
                  do {
                      u8g2.setFont(u8g2_font_smart_patrol_nbp_tf);
                      u8g2.setCursor(0, 10);
                      u8g2.print (band_display);

                      u8g2.setCursor(84, 10);
                      u8g2.print (vfo[OLED_VFO]);
                      u8g2.setCursor(95, 10);
                      u8g2.print (Split);

                      u8g2.setCursor(100, 10);
                      if(rxtxDisplayenable == true){
                      u8g2.print (rxtxmodeDisplay); //freq_inc_name[freq_inc_select] //rxtxmodeDisplay //ModeDisplay
                      }else{
                      u8g2.print(freq_inc_name);
                      }
                      u8g2.setCursor(0, 64);
                      u8g2.print (tuning_step_display); //tuning_step_name                    
                      u8g2.setFont(u8g2_font_logisoso24_tn);
                      u8g2.setCursor(0, 45); //40 
                      u8g2.print (displayFreqMHz, decimal_places);



                      } while ( u8g2.nextPage() );  
                } // end of if statement             
               


}// end of display function

*/

