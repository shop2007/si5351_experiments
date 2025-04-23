/*
 * si5351_sweeper.ino - Si5351 Simple Sweep Generator
 *
 * Copyright (c) 2016 Thomas S. Knutsen <la3pna@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/*
 * Connect Si5351 to I2C
 * Sweep out is on pin 5, ranging from 0-5V (3.3V).
 * Use a filter on sweep out voltage. 100K + 1uF should be a good start.
 * A op-amp can be used to improve the filtering of the DC voltage.
 */

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// VF0-BFO01a 2020-05-28
// Set the frequency of CLK0 and CLK1 of a SI5351a module. CLK0 locked to PLL A. CLK1 locked to PLL B
// Based on the OE1CGS (Christophe) Arduino sketch, modified by Onno, PA2OHH to use it as VFO (CLK0) and BFO (CLK1) for a receiver
// It is controlled by a Python program via RS232.
// The Arduino Nano is connected to the USB port of the PC. The SI5351 module is connected to the +5V, GND, SDA to A04, SCL to A05
// ===
// Usage:
// Command "Annnnnnnn" Set frequency nnnnnnnn of CLK0
// Command "Bnnnnnnnn" Set frequency nnnnnnnn of CLK1
// Command "An" Set power of CLK0, n=0 = off; n=1 = -8 dBm; n=2 = -3 dBm; n=3 = 0 dBm
// Command "Bn" Set power of CLK1, n=0 = off; n=1 = -8 dBm; n=2 = -3 dBm; n=3 = 0 dBm
// ===
// Notes of  OE1CGS (Christophe):
// Covers the full frequency range from 7900 Hz to 200 MHz [See note below]
// The code is in one file, i.e. no messing around with .h and .c
// Allows to select the output power between 4 levels
// Makes use of the wire-library supported by the Arduino IDE for I2C
// As you can see the code specifically adresses coding newbies (like me :))
// as it runs on the Arduino IDE (tested in 1.6.5) without any further hassles.
// It is easy to read as it makes use of the wire-library instead of bit banging the I2C on the code level.
// And I tried my best to write a lot of comments in the lines.... Downside? You'll need the Arduino IDE.



//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//sweep
int correction = 0; // use the Si5351 correction sketch to find the frequency correction factor

int inData = 0;
long steps = 100;
unsigned long  startFreq = 10000000;
unsigned long  stopFreq = 100000000;

int analogpin = 5;
int delaytime = 50;
bool ModoSweep = false;
bool ModoGeneratore = false;

#include <si5351.h>
#include "Wire.h"

Si5351 si5351;


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//gen
#define F_XTAL  25000000                    // Exact frequency of Xtal in Hz, 25 MHz or 27 MHz or correction in User Software
#define c       1048574                     // "c" part of Feedback-Multiplier from XTAL to PLL

String inputString = "";                    // a string to hold incoming data
boolean stringComplete = false;             // whether the string is complete
unsigned long frequencyA = 10000000;        // Frequency CLK0
unsigned long powerA = 2;                   // Power CLK0 0 = off; 1 = -8 dBm; 2 = -3 dBm; 3 = 0 dBm
unsigned long frequencyB = 20000000;               // frequency CLK1
unsigned long powerB = 2;                   // Power CLK1 0 = off; 1 = -8 dBm; 2 = -3 dBm; 3 = 0 dBm
unsigned long newfrequency = 0;             // Just used as variable
unsigned long MS0_P1old = 0;                // Old setting of MS0_P1 (CLK0)
unsigned long MS1_P1old = 0;                // Old setting of MS1_P1 (CLK1)

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void setup()
{
  Wire.begin();                             // Initialize I2C-communication as master  SDA on pin ADC04 -  SCL on pin ADC05

  Serial.begin(115200);
  /*
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  */
  Menu();
  inputString.reserve(50);                  // reserve 50 bytes for the inputString:


}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void Menu()
{
  Serial.println();
  Serial.println("1 = Si5351 Sweeper v5 - onda quadra - terza armonica elevata");
  Serial.println("2 = Si5351 Dual Generator - onda quadra - terza armonica elevata");
  Serial.println("0 Exit modo Sweep");
  
  //Serial.println(delaytime);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void MenuSweep()
{
  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, correction);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_6MA);

  Serial.println();
  Serial.println("Si5351 Square Wave Sweeper v3");
  Serial.print("A10..... = Start f= ");   Serial.println(startFreq);
  Serial.print("B10..... = Stop  f= ");   Serial.println(stopFreq);
  Serial.print("S100 = Step= ");   Serial.println(steps);
  Serial.print("D50 = delaytime= ");   Serial.println(delaytime);
  Serial.println("M = Single sweep");
  Serial.println("P = Single sweep with print");
  Serial.println("C = Repeat sweep until Q");
  Serial.println("T = Timestep in ms, currently ");
  Serial.println("L = List Sweep step ");
  Serial.print("0 Exit mode Sweep");
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void MenuGeneratore()
{
  Serial.println();
  Serial.println("Si5351 Dual Square Wave Generator");
  Serial.println("A0 = Gen.A off");
  Serial.println("A1 = Gen.A Pwr -8dB");
  Serial.println("A2 = Gen.A  Pwr -6dB");
  Serial.println("A3 = Gen.A  Pwr -3dB");
  Serial.println("A4 = Gen.A  Pwr OdB");
  Serial.print("A10..... = Gen.A f= ");   Serial.println(frequencyA);
  Serial.println("B0 = Gen.A off");
  Serial.println("B1 = Gen.A Pwr -8dB");
  Serial.println("B2 = Gen.A  Pwr -6dB");
  Serial.println("B3 = Gen.A  Pwr -3dB");
  Serial.println("B4 = Gen.A  Pwr OdB");
  Serial.print("B10..... = Gen.B f= ");   Serial.println(frequencyB);
  Serial.print("0 Exit mode Generator");
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void loop(){
  serialEvent();


  inData = 0;
  if(Serial.available() > 0)   // see if incoming serial data:
  {
    inData = Serial.read();  // read oldest byte in serial buffer:
  }

  if(inData == '1' )
  {
    ModoSweep=true;
    MenuSweep();
    loopSweep();
  }
  if(inData == '2' )
  {
    ModoGeneratore=true;
    MenuGeneratore();
    loopGen();
  }

}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void loopSweep()
{
  while (ModoSweep){
    serialEvent();
  
    inData = 0;
    if(Serial.available() > 0)   // see if incoming serial data:
    {
      inData = Serial.read();  // read oldest byte in serial buffer:
    }

    if(inData == 'M' || inData == 'm')
    {
      si5351.output_enable(SI5351_CLK0, 1);    
      inData = 0;
      unsigned long freqstep = (stopFreq - startFreq) / steps;
      for(int i = 0; i < (steps + 1); i++ )
      {
        unsigned long freq = startFreq + (freqstep*i);
        si5351.set_freq(freq * SI5351_FREQ_MULT, SI5351_CLK0);
        analogWrite(analogpin, map(i, 0, steps, 0, 255));
        delay(delaytime);
      }
      si5351.output_enable(SI5351_CLK0, 0);
    }

    if(inData == 'P' || inData == 'p')
    {
      inData = 0;
      unsigned long freqstep = (stopFreq - startFreq) / steps;
      si5351.output_enable(SI5351_CLK0, 1);
      for(int i = 0; i < (steps + 1); i++ )
      {
        unsigned long freq = startFreq + (freqstep*i);
        si5351.set_freq(freq * SI5351_FREQ_MULT, SI5351_CLK0);
        Serial.println(freq);
        analogWrite(analogpin, map(i, 0, steps, 0, 255));
        delay(delaytime);
      }
      si5351.output_enable(SI5351_CLK0, 0);
      MenuSweep(); 
    }

    if(inData == 'C' || inData == 'c')
    {
      Serial.println("Q to stop sweep ");
      boolean running = true;
      inData = 0;
      si5351.output_enable(SI5351_CLK0, 1);
      while(running)
      {
        Serial.print(".");
        unsigned long freqstep = (stopFreq - startFreq) / steps;
        for (int i = 0; i < (steps + 1); i++ )
        {
          unsigned long freq = startFreq + (freqstep * i);
          si5351.set_freq(freq * SI5351_FREQ_MULT, SI5351_CLK0);
          analogWrite(analogpin, map(i, 0, steps, 0, 255));
          delay(delaytime);
          if(Serial.available() > 0)   // see if incoming serial data:
          {
            inData = Serial.read();  // read oldest byte in serial buffer:
            if(inData == 'Q' || inData == 'q')
            {
              running = false;
              inData = 0;
            }
          }
        }
      }
      si5351.output_enable(SI5351_CLK0, 0);
      MenuSweep(); 
    }

    if(inData == 'S' || inData == 's')
    {
      steps = Serial.parseInt();
      Serial.print("Steps: ");
      Serial.println(steps);
      inData = 0;
      MenuSweep();    
    }

    if(inData == 'T' || inData == 't')
    {
      delaytime = Serial.parseInt();
      Serial.print("time pr step: ");
      Serial.println(delaytime);
      inData = 0;
    }

    if(inData == 'L' || inData == 'l')
    {
      for (int i = 0; i < (steps+1); i++ )
      {
        // print out the value you read:
        Serial.print(i * 10);
        Serial.print(';');
        Serial.print(steps);
        Serial.print(';');
        Serial.println(-i);
        delay(10);        // delay in between reads for stability
      }
      inData = 0;
    }

    if(inData == 'A' || inData == 'a')
    {
      startFreq = Serial.parseInt();
      Serial.print("Start: ");
      Serial.println(startFreq);
      inData = 0;
      MenuSweep();
    }

    if(inData == 'B' || inData == 'b')
    {
      stopFreq = Serial.parseInt();
      Serial.print("Stop: ");
      Serial.println(stopFreq);
      inData = 0;
      MenuSweep();
    }

    if(inData == 'D' || inData == 'd')
    {
      delaytime = Serial.parseInt();
      Serial.print("delaytime: ");
      Serial.println(delaytime);
      inData = 0;
      MenuSweep();
    }

    if(inData == '0' )
    {
      ModoSweep=false;
    }

  }
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
void loopGen()
{

  Si5351a_Write_Reg (16, 128);              // Power down CLK0
  Si5351a_Write_Reg (17, 128);              // Power down CLK1
  Si5351a_Write_Reg (18, 128);              // Power down CLK2 Necessary, also for 10 pin version
  Si5351a_Write_Reg (19, 128);              // Power down CLK3 to switch off all unused CLKn
  Si5351a_Write_Reg (20, 128);              // Power down CLK4 to reduce supply current
  Si5351a_Write_Reg (21, 128);              // Power down CLK5
  Si5351a_Write_Reg (22, 128);              // Power down CLK6
  Si5351a_Write_Reg (23, 128);              // Power down CLK7

  SetFrequencyA(frequencyA);                // Set TX-Frequency CLK0
  if (frequencyA > 10) {
    SetFrequencyA(powerA);                  // Set CLK0 power
  }

  SetFrequencyB(frequencyB);                // Set TX-Frequency CLK1
  if (frequencyB > 10) {
    SetFrequencyB(powerB);                  // Set CLK1 power
  } 
 
  while (ModoGeneratore){
    serialEvent();

    // process inputString when stringComplete = true:
    if (stringComplete) {
      if (inputString[0] == '?') {
        Serial.println("VF0-BFO01a 2020-05-28\n");
        MenuGeneratore();
      }

      if (inputString[0] == 'A') {
        inputString[0] = '0';
        newfrequency = inputString.toInt();
        SetFrequencyA(newfrequency);
        MenuGeneratore();
      }

      if (inputString[0] == 'B') {
        inputString[0] = '0';
        newfrequency = inputString.toInt();
        SetFrequencyB(newfrequency);
        MenuGeneratore();
      }

      if (inputString[0] == 'Q') {
        ModoGeneratore = false;
      }

      inputString = "";                      // clear the string:
      stringComplete = false;
    }
  }
}

void SetFrequencyA(unsigned long frequency) { // Frequency in Hz; must be within [7810 Hz to 200 MHz]
  unsigned long fvco;                  // VCO frequency (600-900 MHz) of PLL
  unsigned long outdivider;            // Output divider in range [4,6,8-900], even numbers preferred
  byte R = 1;                          // Additional Output Divider in range [1,2,4,...128]
  byte a;                              // "a" part of Feedback-Multiplier from XTAL to PLL in range [15,90]
  unsigned long b;                     // "b" part of Feedback-Multiplier from XTAL to PLL
  float f;                             // floating variable, needed in calculation
  unsigned long MS0_P1;                // Si5351a Output Divider register MS0_P1, P2 and P3 are hardcoded below
  unsigned long MSNA_P1;               // Si5351a Feedback Multisynth register MSNA_P1
  unsigned long MSNA_P2;               // Si5351a Feedback Multisynth register MSNA_P2
  unsigned long MSNA_P3;               // Si5351a Feedback Multisynth register MSNA_P3

  if (frequency == 0) {
    Si5351a_Write_Reg (16, 128);       // Switch off output CLK0
  }
  if (frequency == 1) {
    Si5351a_Write_Reg (16, 76);        // CLK0 drive strength = 2mA; power level ~ -8dB
    Si5351a_Write_Reg (177, 32);       // This resets PLL A
  }
  if (frequency == 2) {
    Si5351a_Write_Reg (16, 77);        // CLK0 drive strength = 4mA; power level ~ -3dB
    Si5351a_Write_Reg (177, 32);       // This resets PLL A
  }
  if (frequency == 3) {
    Si5351a_Write_Reg (16, 78);        // CLK0 drive strength = 6mA; power level ~ -1dB
    Si5351a_Write_Reg (177, 32);       // This resets PLL A
  }
  if (frequency == 4) {
    Si5351a_Write_Reg (16, 79);        // CLK0 drive strength = 8mA; power level := 0dB
    Si5351a_Write_Reg (177, 32);       // This resets PLL A
  }

  if (frequency < 7900) {              // Frequency > 7900 Hz
    return;
  }
  if (frequency > 200000000) {         // Frequency < 200 MHz
    return;
  }

  frequencyA = frequency;

  outdivider = 900000000 / frequency;  // With 900 MHz beeing the maximum internal PLL-Frequency

  while (outdivider > 900) {           // If output divider out of range (>900) use additional Output divider
    R = R * 2;
    outdivider = outdivider / 2;
  }
  
  if (outdivider % 2) outdivider--;    // finds the even divider which delivers the intended Frequency

  fvco = outdivider * R * frequency;   // Calculate the PLL-Frequency (given the even divider)

  switch (R) {                         // Convert the Output Divider to the bit-setting required in register 44
    case 1: R = 0; break;              // Bits [6:4] = 000
    case 2: R = 16; break;             // Bits [6:4] = 001
    case 4: R = 32; break;             // Bits [6:4] = 010
    case 8: R = 48; break;             // Bits [6:4] = 011
    case 16: R = 64; break;            // Bits [6:4] = 100
    case 32: R = 80; break;            // Bits [6:4] = 101
    case 64: R = 96; break;            // Bits [6:4] = 110
    case 128: R = 112; break;          // Bits [6:4] = 111
  }

  a = fvco / F_XTAL;                   // Multiplier to get from Quartz-Oscillator Freq. to PLL-Freq.
  f = fvco - a * F_XTAL;               // Multiplier = a+b/c
  f = f * c;                           // this is just "int" and "float" mathematics
  f = f / F_XTAL;
  b = f;

  MS0_P1 = 128 * outdivider - 512;     // Calculation of Output Divider registers MS0_P1 to MS0_P3
  // MS0_P2 = 0 and MS0_P3 = 1; these values are hardcoded, see below

  f = 128 * b / c;                     // Calculation of Feedback Multisynth registers MSNA_P1 to MSNA_P3
  MSNA_P1 = 128 * a + f - 512;
  MSNA_P2 = f;
  MSNA_P2 = 128 * b - MSNA_P2 * c;
  MSNA_P3 = c;

  // Si5351a_Write_Reg (16, 128);                      // Disable output during the following register settings
  Si5351a_Write_Reg (26, (MSNA_P3 & 65280) >> 8);   // Bits [15:8] of MSNA_P3 in register 26
  Si5351a_Write_Reg (27, MSNA_P3 & 255);            // Bits [7:0]  of MSNA_P3 in register 27
  Si5351a_Write_Reg (28, (MSNA_P1 & 196608) >> 16); // Bits [17:16] of MSNA_P1 in bits [1:0] of register 28
  Si5351a_Write_Reg (29, (MSNA_P1 & 65280) >> 8);   // Bits [15:8]  of MSNA_P1 in register 29
  Si5351a_Write_Reg (30, MSNA_P1 & 255);            // Bits [7:0]  of MSNA_P1 in register 30
  Si5351a_Write_Reg (31, ((MSNA_P3 & 983040) >> 12) | ((MSNA_P2 & 983040) >> 16)); // Parts of MSNA_P3 and MSNA_P1
  Si5351a_Write_Reg (32, (MSNA_P2 & 65280) >> 8);   // Bits [15:8]  of MSNA_P2 in register 32
  Si5351a_Write_Reg (33, MSNA_P2 & 255);            // Bits [7:0]  of MSNA_P2 in register 33

  if (MS0_P1 != MS0_P1old) {
    MS0_P1old = MS0_P1;
    Si5351a_Write_Reg (42, 0);                        // Bits [15:8] of MS0_P3 (always 0) in register 42
    Si5351a_Write_Reg (43, 1);                        // Bits [7:0]  of MS0_P3 (always 1) in register 43
    Si5351a_Write_Reg (44, ((MS0_P1 & 196608) >> 16) | R);  // Bits [17:16] of MS0_P1 in bits [1:0] and R in [7:4]
    Si5351a_Write_Reg (45, (MS0_P1 & 65280) >> 8);    // Bits [15:8]  of MS0_P1 in register 45
    Si5351a_Write_Reg (46, MS0_P1 & 255);             // Bits [7:0]  of MS0_P1 in register 46
    Si5351a_Write_Reg (47, 0);                        // Bits [19:16] of MS0_P2 and MS0_P3 are always 0
    Si5351a_Write_Reg (48, 0);                        // Bits [15:8]  of MS0_P2 are always 0
    Si5351a_Write_Reg (49, 0);                        // Bits [7:0]   of MS0_P2 are always 0

    if (outdivider == 4) {
      Si5351a_Write_Reg (44, 12 | R);                 // Special settings for R = 4 (see datasheet)
      Si5351a_Write_Reg (45, 0);                      // Bits [15:8]  of MS0_P1 must be 0
      Si5351a_Write_Reg (46, 0);                      // Bits [7:0]  of MS0_P1 must be 0
    }
    Si5351a_Write_Reg (177, 32);                      // This resets PLL A
  }
}


void SetFrequencyB(unsigned long frequency) { // Frequency in Hz; must be within [7810 Hz to 200 MHz]
  unsigned long fvco;                  // VCO frequency (600-900 MHz) of PLL
  unsigned long outdivider;            // Output divider in range [4,6,8-900], even numbers preferred
  byte R = 1;                          // Additional Output Divider in range [1,2,4,...128]
  byte a;                              // "a" part of Feedback-Multiplier from XTAL to PLL in range [15,90]
  unsigned long b;                     // "b" part of Feedback-Multiplier from XTAL to PLL
  float f;                             // floating variable, needed in calculation
  unsigned long MS1_P1;                // Si5351a Output Divider register MS1_P1, P2 and P3 are hardcoded below
  unsigned long MSNB_P1;               // Si5351a Feedback Multisynth register MSNB_P1
  unsigned long MSNB_P2;               // Si5351a Feedback Multisynth register MSNB_P2
  unsigned long MSNB_P3;               // Si5351a Feedback Multisynth register MSNB_P3

  if (frequency == 0) {
    Si5351a_Write_Reg (17, 128);       // Switch off output CLK1
  }
  if (frequency == 1) {
    Si5351a_Write_Reg (17, 108);       // CLK0 drive strength = 2mA; power level ~ -8dB
    Si5351a_Write_Reg (177, 128);      // This resets PLL B
  }
  if (frequency == 2) {
    Si5351a_Write_Reg (17, 109);       // CLK1 drive strength = 4mA; power level ~ -3dB
    Si5351a_Write_Reg (177, 128);      // This resets PLL B
  }
  if (frequency == 3) {
    Si5351a_Write_Reg (17, 110);       // CLK1 drive strength = 6mA; power level ~ -1dB
    Si5351a_Write_Reg (177, 128);      // This resets PLL B
  }
  if (frequency == 4) {
    Si5351a_Write_Reg (17, 111);       // CLK1 drive strength = 8mA; power level := 0dB
    Si5351a_Write_Reg (177, 128);      // This resets PLL B
  }

  if (frequency < 7900) {              // Frequency > 7900 Hz
    return;
  }
  if (frequency > 200000000) {         // Frequency < 200 MHz
    return;
  }

  frequencyB = frequency;

  outdivider = 900000000 / frequency;  // With 900 MHz beeing the maximum internal PLL-Frequency

  while (outdivider > 900) {           // If output divider out of range (>900) use additional Output divider
    R = R * 2;
    outdivider = outdivider / 2;
  }
  if (outdivider % 2) outdivider--;    // finds the even divider which delivers the intended Frequency

  fvco = outdivider * R * frequency;   // Calculate the PLL-Frequency (given the even divider)

  switch (R) {                         // Convert the Output Divider to the bit-setting required in register 44
    case 1: R = 0; break;              // Bits [6:4] = 000
    case 2: R = 16; break;             // Bits [6:4] = 001
    case 4: R = 32; break;             // Bits [6:4] = 010
    case 8: R = 48; break;             // Bits [6:4] = 011
    case 16: R = 64; break;            // Bits [6:4] = 100
    case 32: R = 80; break;            // Bits [6:4] = 101
    case 64: R = 96; break;            // Bits [6:4] = 110
    case 128: R = 112; break;          // Bits [6:4] = 111
  }

  a = fvco / F_XTAL;                   // Multiplier to get from Quartz-Oscillator Freq. to PLL-Freq.
  f = fvco - a * F_XTAL;               // Multiplier = a+b/c
  f = f * c;                           // this is just "int" and "float" mathematics
  f = f / F_XTAL;
  b = f;

  MS1_P1 = 128 * outdivider - 512;     // Calculation of Output Divider registers MS0_P1 to MS0_P3
  // MS1_P2 = 0 and MS1_P3 = 1; these values are hardcoded, see below

  f = 128 * b / c;                     // Calculation of Feedback Multisynth registers MSNA_P1 to MSNA_P3
  MSNB_P1 = 128 * a + f - 512;
  MSNB_P2 = f;
  MSNB_P2 = 128 * b - MSNB_P2 * c;
  MSNB_P3 = c;

  // Si5351a_Write_Reg (17, 128);                      // Disable output during the following register settings
  Si5351a_Write_Reg (34, (MSNB_P3 & 65280) >> 8);   // Bits [15:8] of MSNB_P3 in register 34
  Si5351a_Write_Reg (35, MSNB_P3 & 255);            // Bits [7:0]  of MSNB_P3 in register 35
  Si5351a_Write_Reg (36, (MSNB_P1 & 196608) >> 16); // Bits [17:16] of MSNB_P1 in bits [1:0] in register 36
  Si5351a_Write_Reg (37, (MSNB_P1 & 65280) >> 8);   // Bits [15:8]  of MSNB_P1 in register 37
  Si5351a_Write_Reg (38, MSNB_P1 & 255);            // Bits [7:0]  of MSNB_P1 in register 38
  Si5351a_Write_Reg (39, ((MSNB_P3 & 983040) >> 12) | ((MSNB_P2 & 983040) >> 16)); // Parts of MSNB_P3 and MSNB_P1 in register 39
  Si5351a_Write_Reg (40, (MSNB_P2 & 65280) >> 8);   // Bits [15:8]  of MSNB_P2 in register 40
  Si5351a_Write_Reg (41, MSNB_P2 & 255);            // Bits [7:0]  of MSNB_P2 in register 41

  if (MS1_P1 != MS1_P1old) {
    MS1_P1old = MS1_P1;
    Si5351a_Write_Reg (50, 0);                        // Bits [15:8] of MS1_P3 (always 0) in register 50 Bits [15:8]
    Si5351a_Write_Reg (51, 1);                        // Bits [7:0]  of MS1_P3 (always 1) in register 51
    Si5351a_Write_Reg (52, ((MS1_P1 & 196608) >> 16) | R);  // Bits [17:16] of MS1_P1 in bits [1:0] and R in [7:4]
    Si5351a_Write_Reg (53, (MS1_P1 & 65280) >> 8);    // Bits [15:8]  of MS1_P1 in register 53
    Si5351a_Write_Reg (54, MS1_P1 & 255);             // Bits [7:0]  of MS1_P1 in register 54
    Si5351a_Write_Reg (55, 0);                        // Bits [19:16] of MS1_P2 and MS1_P3 are always 0
    Si5351a_Write_Reg (56, 0);                        // Bits [15:8]  of MS1_P2 are always 0
    Si5351a_Write_Reg (57, 0);                        // Bits [7:0]   of MS1_P2 are always 0

    if (outdivider == 4) {
      Si5351a_Write_Reg (60, 12 | R);                 // Special settings for R = 4 (see datasheet)
      Si5351a_Write_Reg (61, 0);                      // Bits [15:8]  of MS1_P1 must be 0
      Si5351a_Write_Reg (62, 0);                      // Bits [7:0]  of MS1_P1 must be 0
    }
    Si5351a_Write_Reg (177, 128);                     // This resets PLL B
  }
}


void Si5351a_Write_Reg (byte regist, byte value) {  // Writes "byte" into "regist" of Si5351a via I2C
  Wire.beginTransmission(96);                       // Starts transmission as master to slave 96, which is the
  // I2C address of the Si5351a (see Si5351a datasheet)
  Wire.write(regist);                               // Writes a byte containing the number of the register
  Wire.write(value);                                // Writes a byte containing the value to be written in the register
  Wire.endTransmission();                           // Sends the data and ends the transmission
}

/*
  SerialEvent occurs whenever a new data comes in the
  hardware serial RX.  This routine is run between each
  time loop() runs, so using delay inside loop can delay
  response.  Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    if (inChar > 32) {
      inputString += inChar;
    }
    else {
      stringComplete = true;
    }
  }
}
 








