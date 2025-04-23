/*
SERVE TERMINALE 115200
USCITA 3

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

int correction = 0; // use the Si5351 correction sketch to find the frequency correction factor

int inData = 0;
long steps = 100;
unsigned long  startFreq = 10000000;
unsigned long  stopFreq = 100000000;
unsigned long  Generatore2 = 1;
unsigned long  Generatore3 = 1;

#define PLLB_FREQ    87600000000ULL

int analogpin = 11;
int delaytime = 50;

#include <si5351.h>
#include "Wire.h"

Si5351 si5351;


void setup()
{
  Serial.begin(115200);
  /*
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  */
  info();
}

void info()
{
  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, correction);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_6MA);

  Serial.println("Si5351 Sweeper");
  Serial.print("A1000000 = Start f= ");   Serial.println(startFreq);
  Serial.print("B10000000 = Stop  f= ");   Serial.println(stopFreq);
  Serial.print("S100 = Step= ");   Serial.println(steps);
  Serial.print("D50 = delaytime= ");   Serial.println(delaytime);
  Serial.println("M = Single sweep");
  Serial.println("P = Single sweep with print");
  Serial.println("C = Continious sweep until Q");
  Serial.println("T = Timestep in ms, currently ");
  Serial.println("L = List step ");Serial.println("X = Generator 2 frequency");
  
  Serial.println("-");
  Serial.println("X2000000 = Generator 2 2000000 Hz");
  Serial.println("Z = Generator 2 frequency");
    
  //Serial.println(delaytime);
}


void loop()
{
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
    info(); 
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
    info(); 
  }

  if(inData == 'S' || inData == 's')
  {
    steps = Serial.parseInt();
    Serial.print("Steps: ");
    Serial.println(steps);
    inData = 0;
    info();    
  }

  if(inData == 'H' || inData == 'h')
  {
    info();
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
    info();
  }

  if(inData == 'B' || inData == 'b')
  {
    stopFreq = Serial.parseInt();
    Serial.print("Stop: ");
    Serial.println(stopFreq);
    inData = 0;
    info();
  }

  if(inData == 'D' || inData == 'd')
  {
    delaytime = Serial.parseInt();
    Serial.print("delaytime: ");
    Serial.println(delaytime);
    inData = 0;
    info();
  }



  if(inData == 'X' || inData == 'x')
  {
     //Serial.print("Generatore2: ");
      // Tune to 146 MHz center frequency
    //si5351.set_freq_manual(200000000ULL, PLLB_FREQ, SI5351_CLK1);

    //si5351.update_status(); 
    /*
    Generatore2 = Serial.parseInt();
    Serial.print("Generatore2: ");
    Serial.println(Generatore2);
    */
    inData = 0;
    info();
  }



  if(inData == 'X' || inData == 'x')
  {
    si5351.output_enable(SI5351_CLK1, 0);
    inData = 0;
    info();
  }









}
