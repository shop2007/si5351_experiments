#ifndef SI5351_CLK_GEN_H
//#define SI5351_CLK_GEN_H

#include "si5351x.h"

class SI5351_CLK_GEN{

public:
void begin(int Cal_si5351);
void CLK0_FreqGen(long GenFreq);

private:
Si5351 si5351;
long m_GenFreq;
unsigned long long clk_gen_freq;

}; // end of class
#endif //SI5351_CLK_GEN_H

