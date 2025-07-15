#ifndef MAX7219_H
//#define MAX7219_H

#define DIN 47
#define CS 21
#define CLK 38
#include "LedController.hpp"


class MAX7219_X{

public:
void begin(uint8_t LED_brightness);
void MAX7219_display(long Main_Display_FreqHz, long Alt_Display_FreqH, bool vfosplitenabled);

private:


LedController<1,1> lc; // creating object
bool freq_range[3]{true,true,true};
long freq_rangeHz[3]{1e6,10e6,100e6};
bool point_marker[9]{false, false, false, false, false, false, false, false, false}; 
long m_freqHz;
long m_Main_Display_FreqHz; 
long m_Alt_Display_FreqH; 
bool m_vfosplitenabled;


};//end of MAX7219_X class
#endif //MAX7219_H