#ifndef SSD1306_DISPLAY_H
//#define SSD1306_DISPLAY_H
#include <U8g2lib.h>
#define vfo_A 1 
#define vfo_B 2 

class SSD1306_DISPLAY{

public:
void begin();
void Display(String band_display, bool rxtxDisplayenable, String rxtxmodeDisplay, String freq_inc_name, String tuning_step_display, float displayFreqMHz, uint8_t decimal_places, bool chanelmodenabled, int channel, uint8_t chan_band_select, uint8_t OLED_VFO, bool vfosplitenabled);



void dispaly_freq(long Alt_Display_FreqHz, long Main_Display_FreqHz, bool TX8mode, bool vfosplitenabled, bool chanelmodenabled);
void display_freq_res(uint8_t oneHzRes, bool oneHz);
void display_tstep_speed(String tuning_step, String tuning_speed);
void display_band_mode(String BandDisplay, String ModeDisplay, bool TX8mode, bool modeDisplayenable, bool rxtxDisplayenable);
void display_vfo(uint8_t vfo_Display);
void display_channel_mode(bool chanelmodenabled, int vfo_A_chan_num, int vfo_B_chan_num);

private:

void VFO_OLED_SSD1306();
float displayFreqMHz;
float m_displayFreqMHz;
uint8_t decimal_places;
uint8_t m_oneHzRes;
bool m_oneHz;
uint8_t freq_inc_select;
bool update_freq_dispaly = false;
String tuning_step_display;
String m_tuning_step_display;
String band_display;
String m_band_display;
String rxtxmodeDisplay;
String m_freq_inc_name;
bool m_modeDisplayenable;
bool m_rxtxDisplayenable;
bool m_TX8mode;
String freq_inc_name[2]{"KHz", "MHz"};
String ChannelFreq;
String channelNum;

String vfo[3]{" ","A","B"};
String Split;
uint8_t m_vfo_Display;
uint8_t test;

bool m_vfosplitenabled = false;
int display_chan;
int m_display_chan;
bool m_chanelmodenabled;
String m_BandDisplay;

};
#endif //SSD1306_DISPLAY_H

