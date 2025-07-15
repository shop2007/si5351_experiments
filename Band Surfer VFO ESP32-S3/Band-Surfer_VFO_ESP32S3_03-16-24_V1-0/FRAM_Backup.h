#ifndef FRAM_BACKUP_H
//#define FRAM_BACKUP_H

#include "Adafruit_FRAM_I2C.h"
#include <Wire.h>
#define SDA0_Pin 40 //8   // select ESP32  I2C pins
#define SCL0_Pin 39 //9

class FRAM_MEM{

public:
void begin(char FRAM_I2C_address);
long init_band_freq_mem_A(bool reset, int select, long band_freqHz_A);
long init_band_freq_mem_B(bool reset, int select, long band_freqHz_B);
int init_general_global_settings(bool reset, int select, int gen_setting);

int init_channel_set_A(bool reset, int select, int channel_num_A);
int init_channel_set_B(bool reset, int select, int channel_num_B);




void band_freq_mem_A( int select, long band_freqHz_A);
void band_freq_mem_B( int select, long band_freqHz_B);

void channel_set_A(int select, int channel_num_A);
void channel_set_B(int select, int channel_num_B);



void general_global_settings(int select, int gen_setting);


int init_chan_general_settings(bool reset, int select, int chan_setting);
void chan_general_settings(int select, int chan_setting);

private:
Adafruit_FRAM_I2C fram;
uint8_t byte_size;
int band_start_address_A = 100;
int band_start_address_B = 200;
int gen_set_start_address = 300;
int chan_band_start_address_A = 400;
int chan_band_start_address_B = 500;
int chan_gen_set_start_address = 600;

int m_channel_num_A;
int m_channel_num_B;

int memory_address;

long m_band_freqHz_A;
long m_band_freqHz_B;

int m_gen_setting;


int m_chan_setting;


long mm_band_freqHz_A;
long mm_band_freqHz_B;
unsigned long millis_band_A; 
unsigned long millis_band_B;
int freq_write_timer = 1500; 
};//end of class
#endif //FRAM_BACKUP_H