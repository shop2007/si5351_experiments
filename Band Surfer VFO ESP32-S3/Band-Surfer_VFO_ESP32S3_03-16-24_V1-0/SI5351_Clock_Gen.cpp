#include "SI5351_Clock_Gen.h"

void SI5351_CLK_GEN::begin(int Cal_si5351){
    si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
    si5351.set_correction(122360, SI5351_PLL_INPUT_XO); //calibration for si5351 //122360
    si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA); //RF output level adjust
    si5351.update_status();
}//end of begin function

void SI5351_CLK_GEN::CLK0_FreqGen(long GenFreq){  
    if(GenFreq >= 140e6){GenFreq = 140e6;}//Limiter
    if(GenFreq <= 8e3){GenFreq = 8e3;}//Limiter
    if(m_GenFreq != GenFreq){
    //Serial.println(GenFreq);
    clk_gen_freq = (unsigned long long) GenFreq;
    
    si5351.set_freq(clk_gen_freq *1e2,  SI5351_CLK0); //1400000000ULL
    m_GenFreq = GenFreq;
    
    }
}//end of CLK0_FreqGen function