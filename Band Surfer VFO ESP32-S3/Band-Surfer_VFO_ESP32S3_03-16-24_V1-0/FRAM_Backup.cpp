#include "FRAM_Backup.h"

void FRAM_MEM::begin(char FRAM_I2C_address){
//fram.begin(0x57);

  Wire.begin(SDA0_Pin, SCL0_Pin);
  //fram.begin(0x57);
 
  if (fram.begin(FRAM_I2C_address)) {  // you can stick the new i2c addr in here, e.g. begin(0x51);
    //Serial.println("Found I2C FRAM");
  } else {
    //Serial.println("I2C FRAM not identified ... check your connections?\r\n");
    //Serial.println("Will continue in case this processor doesn't support repeated start\r\n");
    while (1);
  }
}//end of begin function
//*******************************************************************************
//*******************************************************************************
//*******************************************************************************
long FRAM_MEM::init_band_freq_mem_A(bool reset, int select, long band_freqHz_A){
byte_size = 4;
memory_address = select * byte_size + band_start_address_A;
if(reset == true){
fram.write(memory_address, (uint8_t*) &band_freqHz_A, sizeof(2));
m_band_freqHz_A = band_freqHz_A;
}else{  
fram.read(memory_address, (uint8_t*) &m_band_freqHz_A, sizeof(m_band_freqHz_A));}
return m_band_freqHz_A;
}//end of Band_Freq_Mem function
//*******************************************************************************
//*******************************************************************************
//*******************************************************************************
long FRAM_MEM::init_band_freq_mem_B(bool reset, int select, long band_freqHz_B){
byte_size = 4; 
memory_address = select * byte_size + band_start_address_B;
if(reset == true){
fram.write(memory_address, (uint8_t*) &band_freqHz_B, sizeof(2));
m_band_freqHz_B = band_freqHz_B;
}else{  
fram.read(memory_address, (uint8_t*) &m_band_freqHz_B, sizeof(m_band_freqHz_B));}
return m_band_freqHz_B;
}//end of Band_Freq_Mem function
//*******************************************************************************
//*******************************************************************************
//*******************************************************************************
int FRAM_MEM::init_general_global_settings(bool reset, int select, int gen_setting){
byte_size = 4;
memory_address = select * byte_size + gen_set_start_address;
if(reset == true){
fram.write(memory_address, (uint8_t*) &gen_setting, sizeof(2));
m_gen_setting = gen_setting;
}else{
fram.read(memory_address, (uint8_t*) &m_gen_setting, sizeof(m_gen_setting));}
return m_gen_setting;
}//end of general_global_settings function
//*******************************************************************************
//*******************************************************************************
//*******************************************************************************
void FRAM_MEM::general_global_settings(int select, int gen_setting){
byte_size = 4;
memory_address = select * byte_size + gen_set_start_address;
fram.write(memory_address, (uint8_t*) &gen_setting, sizeof(2));
}//end of general_global_settings funtion
//*******************************************************************************
//*******************************************************************************
//*******************************************************************************
void FRAM_MEM::band_freq_mem_A( int select, long band_freqHz_A){  
byte_size = 4;
memory_address = select * byte_size + band_start_address_A;            
if(mm_band_freqHz_A != band_freqHz_A && millis() - millis_band_A > freq_write_timer){//freq_write_timer
fram.write(memory_address, (uint8_t*) &band_freqHz_A, sizeof(2));
millis_band_A = millis();
mm_band_freqHz_A = band_freqHz_A;
}
}//end of band_freq_mem_A function

//*******************************************************************************
//*******************************************************************************
//*******************************************************************************
void FRAM_MEM::band_freq_mem_B( int select, long band_freqHz_B){
byte_size = 4;
memory_address = select * byte_size + band_start_address_B;            
if(mm_band_freqHz_B != band_freqHz_B && millis() - millis_band_B > freq_write_timer){
fram.write(memory_address, (uint8_t*) &band_freqHz_B, sizeof(2));
millis_band_B = millis();
mm_band_freqHz_B = band_freqHz_B;
}
}//end of band_freq_mem_B function
//*******************************************************************************
//*******************************************************************************
//*******************************************************************************
int FRAM_MEM::init_channel_set_A(bool reset, int select, int channel_num_A){
byte_size = 4;
memory_address = select * byte_size + chan_band_start_address_A;
if(reset == true){
fram.write(memory_address, (uint8_t*) &channel_num_A, sizeof(2));
m_channel_num_A = channel_num_A;
}else{  
fram.read(memory_address, (uint8_t*) &m_channel_num_A, sizeof(m_channel_num_A));}
return m_channel_num_A;
}// end of channel_set_A fuction
//*******************************************************************************
//*******************************************************************************
//*******************************************************************************
int FRAM_MEM::init_channel_set_B(bool reset, int select, int channel_num_B){
byte_size = 4;
memory_address = select * byte_size + chan_band_start_address_B;
if(reset == true){
fram.write(memory_address, (uint8_t*) &channel_num_B, sizeof(2));
m_channel_num_B = channel_num_B;
}else{  
fram.read(memory_address, (uint8_t*) &m_channel_num_B, sizeof(m_channel_num_B));} 
return m_channel_num_B;
}// end of channel_set_B fuction
//*******************************************************************************
//*******************************************************************************
//*******************************************************************************

int FRAM_MEM::init_chan_general_settings(bool reset, int select, int chan_setting){
byte_size = 4;
memory_address = select * byte_size + chan_gen_set_start_address;
if(reset == true){
fram.write(memory_address, (uint8_t*) &chan_setting, sizeof(2));
m_chan_setting = chan_setting;
}else{
fram.read(memory_address, (uint8_t*) &m_chan_setting, sizeof(m_chan_setting));}
return m_chan_setting;
}//end of chan_general_settings fuction

//*******************************************************************************
//*******************************************************************************
//*******************************************************************************



void FRAM_MEM::chan_general_settings(int select, int chan_setting){
byte_size = 4;
memory_address = select * byte_size + chan_gen_set_start_address;
fram.write(memory_address, (uint8_t*) &chan_setting, sizeof(2));

}//end of chan_general_settings fuction




//*******************************************************************************
//*******************************************************************************
//*******************************************************************************
void FRAM_MEM::channel_set_A(int select, int channel_num_A){
byte_size = 4;
memory_address = select * byte_size + chan_band_start_address_A;
fram.write(memory_address, (uint8_t*) &channel_num_A, sizeof(2));
}//end of channel_set_A function
//*******************************************************************************
//*******************************************************************************
//*******************************************************************************
void FRAM_MEM::channel_set_B(int select, int channel_num_B){
byte_size = 4;
memory_address = select * byte_size + chan_band_start_address_B;
fram.write(memory_address, (uint8_t*) &channel_num_B, sizeof(2));
}//end of channel_set_B function
//*******************************************************************************
//*******************************************************************************
//*******************************************************************************









