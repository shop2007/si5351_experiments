#include <LiquidCrystal_I2C.h>
#include<AD9833.h>

#define rootOfTen (sqrt(sqrt(10)))

LiquidCrystal_I2C lcd(0x27, 16, 2);

byte squareWaveChar[8] = {
  0b01110,
  0b01010,
  0b01010,
  0b01010,
  0b01010,
  0b01010,
  0b01010,
  0b11011
};

byte sineWaveChar[8] = {
  0b00000,
  0b00000,
  0b00100,
  0b01010,
  0b01010,
  0b10001,
  0b00000,
  0b00000
};

byte triangleWaveChar[8] = {
  0b00000,
  0b00100,
  0b00100,
  0b01010,
  0b01010,
  0b10001,
  0b10001,
  0b00000
};

AD9833 gen(9);

int startBtn = 6;
bool startStatus = false;

//these pins can not be changed 2/3 are special pins
int encoderPin1 = 2;
int encoderPin2 = 3;

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

long lastencoderValue = 0;

int lastMSB = 0;
int lastLSB = 0;

int encoderBtn = 4;
int waveChangerBtn = 5;
int wave = 0;

int menu = 0;

volatile double precisionScale = 1; //Hz Scale

void setup() {

  Serial.begin (9600);

  lcd.init();
  lcd.backlight();

  startupScreen();

  lcd.createChar(0, squareWaveChar); // create a new custom character (index 0)
  lcd.createChar(1, sineWaveChar); // create a new custom character (index 1)
  lcd.createChar(2, triangleWaveChar); // create a new custom character (index 2)

  pinMode(encoderPin1, INPUT_PULLUP);
  pinMode(encoderPin2, INPUT_PULLUP);
  pinMode(encoderBtn, INPUT_PULLUP);
  pinMode(waveChangerBtn, INPUT_PULLUP);
  pinMode(startBtn, INPUT_PULLUP);

  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on

  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3)
  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);

  gen.Begin();
}

void startupScreen() {
  lcd.setCursor(0, 0);
  lcd.print("Signal Generator");
  lcd.setCursor(0, 1);
  lcd.print(" AV - 19.09.2021");
  delay(2000);
  lcd.clear();
}

void loop() {
  //Do stuff here
  
  //handleSerial();
  //Serial.println(analogRead(A0)); //serial plotter

  if (digitalRead(encoderBtn) == LOW) {
    changeEncoderMenu();
  }

  startFunc();

  if (menu == 0) {
    lcd.setCursor(0, 1);
    lcd.print("Freq: ");
    lcd.print(encoderValue / 4);
    lcd.print("Hz");
    lcd.print("        ");
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.print("Scale: ");
    lcd.print(precisionScale, 0);
    lcd.print("Hz");
    lcd.print("       ");
  }

  if (digitalRead(waveChangerBtn) == LOW) {
    wave ++;
    wave %= 3;
  }

  lcd.setCursor(5, 0);
  lcd.print("Wave: ");
  switch (wave) {
    case 0:
      {
        //Serial.println("Square wave selected");
        lcd.write((byte)0);
        lcd.write((byte)0);
        break;
      }
    case 1:
      {
        //Serial.println("Sine wave selected");
        lcd.write((byte)1);
        lcd.write((byte)1);
        break;
      }
    case 2:
      {
        //Serial.println("Triangle wave selected");
        lcd.write((byte)2);
        lcd.write((byte)2);
        break;
      }
    default:
      break;
  }

  if (startStatus == false) {
    gen.EnableOutput(false);
    lcd.setCursor(0, 0);
    lcd.print("OFF");
  }
  else if (startStatus == true && encoderValue != 0)
  {
    gen.EnableOutput(true);
    lcd.setCursor(0, 0);
    lcd.print("ON ");

    switch (wave) {
      case 0:
        {
          gen.ApplySignal(SQUARE_WAVE, REG0, encoderValue / 4);
          break;
        }
      case 1:
        {
          gen.ApplySignal(SINE_WAVE, REG0, encoderValue / 4);
          break;
        }
      case 2:
        {
          gen.ApplySignal(TRIANGLE_WAVE, REG0, encoderValue / 4);
          break;
        }
      default:
        break;
    }

  }

  delay(200); //just here to slow down the output, and show it will work  even during a delay
}

void handleSerial() {
  while (Serial.available() > 0) {
    char* readFreqChar = Serial.read();
    int readFreq = atoi(readFreqChar);
    encoderValue = readFreq * 4;
    Serial.print("Command received. New frequency:");
    Serial.print(readFreq);
    //Serial.clear();
   }
  
}

void startFunc()
{
  if (digitalRead(startBtn) == LOW) {
    startStatus = not startStatus;
  }
}
void changeEncoderMenu()
{
  //0 default rotation
  //1 precision rotation
  lcd.setCursor(0, 1);
  lcd.print("                    ");
  if (menu == 0)
    menu = 1;
  else
    menu = 0;
}

void updateEncoder() {
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if (menu == 0)
  {
    if (sum == 13 || sum == 4 || sum == 2 || sum == 11)
      encoderValue += 1 * precisionScale;
    if (encoderValue > 12500000 * 4)
      encoderValue = 12500000 * 4;
    if (sum == 14 || sum == 7 || sum == 1 || sum == 8 ) {
      encoderValue -= 1 * precisionScale;
      if (encoderValue < 0 ) {
        encoderValue = 0;
      }
    }

  }
  else
  {
    if (sum == 13 || sum == 4 || sum == 2 || sum == 11)
    {
      if (precisionScale <= 1000000)
        precisionScale *= rootOfTen;
    }
    if (sum == 14 || sum == 7 || sum == 1 || sum == 8 )
    {
      if (precisionScale > 1) {
        precisionScale /= rootOfTen;
      }
    }

  }
  lastEncoded = encoded; //store this value for next time
}
