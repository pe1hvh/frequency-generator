/**********************************************************************************************************
  10kHz to 225MHz VFO / RF Generator with Si5351 and Arduino Nano, with Intermediate
  Frequency (IF) offset (+ or -), RX/TX Selector for QRP Transceivers, Band Presets
  and Bargraph S-Meter. See the schematics for wiring and README.txt for details.
  Based on source 
  By J. CesarSound - ver 2.0 - Feb/2021.
  Forked by JA van Hernen
   - re-code: standard lowerCamelCase nameconvention
   - standard indentation
   - standard formating
   - cleancode (so far as possible)
   - comment code
***********************************************************************************************************/

/***************************************************************************************/
/* Libraries                                                                           */
/***************************************************************************************/
#include <Wire.h>             // Allows the communication between devices or sensors connected via Two Wire Interface Bus. Specific implementation for nRF52. This Library is needed for si5341 https://docs.arduino.cc/language-reference/en/functions/communication/Wire/
#include <Rotary.h>           // Arduino library for reading rotary directions that output a 2-bit gray code. Ben  Buxton https://github.com/brianlow/Rotary
#include <si5351.h>           // A full-featured library for the Si5351 series of clock generator ICs from Silicon Labs  https://github.com/etherkit/Si5351Arduino
#include <Adafruit_GFX.h>     // Adafruit GFX graphics core library, this is the 'core' class that all our other graphics libraries derive from. https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_SSD1306.h> // SSD1306 oled driver library for monochrome 128x64 and 128x32 displays https://github.com/adafruit/Adafruit_SSD1306

/***************************************************************************************/
/* Global constanten (User Preferences)                                                */
/***************************************************************************************/
#define IF         455       //Enter your IF frequency, ex: 455 = 455kHz, 10700 = 10.7MHz, 0 = to direct convert receiver or RF generator, + will add and - will subtract IF offfset.
#define BAND_INIT  7         //Enter your initial Band (1-21) at startup, ex: 1 = frequencyGenerator, 2 = 800kHz (MW), 7 = 7.2MHz (40m), 11 = 14.1MHz (20m). 
#define XT_CAL_F   33000     //Si5351 calibration factor, adjust to get exatcly 10MHz. Increasing this value will decreases the frequency and vice versa.
#define S_GAIN     303       //Adjust the sensitivity of Signal Meter A/D input: 101 = 500mv; 202 = 1v; 303 = 1.5v; 404 = 2v; 505 = 2.5v; 1010 = 5v (max).
#define tunestep   A0        //The pin used by tune step push button.
#define band       A1        //The pin used by band selector push button.
#define rx_tx      A2        //The pin used by RX / TX selector switch, RX = switch open, TX = switch closed to GND. When in TX, the IF value is not considered.
#define adc        A3        //The pin used by Signal Meter A/D input.

/***************************************************************************************/
/* Global Objecten                                                                     */
/***************************************************************************************/
Rotary r = Rotary(2, 3);                                     // Initialize Rotary opbject r (Type Rotary) a. assign pin 2 and pin 3 b. set Initial state c. set default inverter
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire); // Initialize Adafruit_SSD1306 object (Type Adafruit_SSD1306) width 128 pixels hight 64 pixels, using the Wire object(library) by reference
Si5351 si5351(0x60);                                         // Initialize Si5351 object with I2C Address 0x60 (Standard IC2 bus adres for Si5251.

/***************************************************************************************/
/* Global Variabelen                                                                   */
/***************************************************************************************/
unsigned long frequency;           // Global: The frequency for setting the correct clock frequency possible values ex:  100000,   800000,  1800000,  3650000,  4985000 etc
unsigned long frequencyPrevious;   // Global: The previous frequency
unsigned long frequencyStep;       // Global: The frequency step ex: 1, 10, 1000, 5000, 10000
char*         frequencyDisplay;    // Global: MHz or kHz

unsigned long timeNow = 0;          // Global: The current time
unsigned int period = 100;          // Global: Used for calculation the performance time .

long  interFrequency = IF;          // Global: The default interFrequency ex: 455 = 455kHz, 10700 = 10.7MHz,  0 = to direct convert receiver or RF generator, + will add and - will subtract IF  offfset.
long  interFrequencyPrevious = 0;   // Global: The previous interFrequency
char* interFrequencyTypeDisplay ;   // Global: VFO or L O

unsigned int signalMeter;           // Global: Value set by the pin for Signal Meter A/D input.
byte signalMeterRemap;              // Global: The re-map value, set in readSignal().
byte signalMeterRemapPrevious;      // Global: The previous re-map value, set in readSignal().

byte  rotaryDirection = 1;          // Global: Value set by the rotary. Given the direction 1 = clockwise -1 = bandSelectorValueer clockwise
byte  tuneStepValue=4;              // Global: Value set by the tune step push button
char* tuneStepDisplay = " 1kHz";    // Global: The frequency step ex: " 1MHz", "  1Hz", " 10Hz", " 1kHz" ," 5kHz" , "10kHz"
byte  tunePointer = 1;              // Global: Up/Down graph tune pointer, set by set_frequency()

byte  bandSelectorValue = BAND_INIT;// Global: Default set by BAND_INIT otherwise set by the  band selector push button.
char* bandSelectorTypeDisplay;      // Global: The band name for the Display. ex: GEN, MW, 160m 80m etc.

bool  rxtxSwitch = false;           // Global: The status of the  RX / TX selector switch
char* rxtxDisplay = "RX";           // Global: RX or TX  

long calibrationFactor = XT_CAL_F;  // Global: Si5351 calibration factor, adjust to get exatcly 10MHz. Increasing this value will decreases the frequency and vice versa.

/***************************************************************************************/
/*! @brief  Set the frequency started by de interupt handler.
    @param  direction given direction of the rotarysteps:
       1 = clockwise step
      -1 = bandSelectorValueer clockwise step
*/
/*************************************************************************************/
void setFrequency(short dir) {
  if (rotaryDirection == 1) { 
     //Up/Down frequency                       
    if (dir == 1) frequency= frequency+ frequencyStep;
    if (frequency>= 225000000) frequency= 225000000;
    if (dir == -1) frequency= frequency- frequencyStep;
    if (frequencyStep == 1000000 && frequency<= 1000000) frequency= 1000000;
    else if (frequency< 10000) frequency= 10000;
    //Up/Down graph tune pointer
    if (dir == 1) tunePointer = tunePointer + 1;
    if (tunePointer > 42) tunePointer = 1;
    if (dir == -1) tunePointer = tunePointer - 1;
    if (tunePointer < 1) tunePointer = 42;
  }
}

/***************************************************************************************/
/*!
  @brief  Interrupt Handler: Handels a group of Interupt vectors for
  @param  PCINT2_vect interrupt vector for PORTD
*/
/*************************************************************************************/
ISR(PCINT2_vect) {                            // Interrup Service Routine PCINT2_vect: interrupt vector for PORTD
  char result = r.process();                  // a.Grab state of input pins. b.Determine new state from the pins and state table. c.Return emit bits, ie the generated event.
  if (result == DIR_CW)  {                    // DIR_CW = Direction Clockwise step.
    setFrequency(1);
  } else if (result == DIR_CCW)  {            // DIR_CCW = Direction outer-clockwise step.
    setFrequency(-1);
  }
}

/**************************************************************************************/
/*! @brief  Set the frequentie for clock0 using 
         -global frequency and 
         -global interFrequency 
*/
/**************************************************************************************/
void setSi5251Frequency() {
  si5351.set_freq((frequency + (interFrequency* 1000ULL)) * 100ULL, SI5351_CLK0);
}

/***********************************************************************************************************************/
/*! @brief  Set step to new step  and frequency steps to new frequency step depending on global variable tuneStepValue */
/***********************************************************************************************************************/
void setStep() {
  switch (tuneStepValue) {
    case 1: tuneStepValue = 2; frequencyStep = 1;       tuneStepDisplay=(char*)"  1Hz"; break;
    case 2: tuneStepValue = 3; frequencyStep = 10;      tuneStepDisplay=(char*)" 10Hz";  break;
    case 3: tuneStepValue = 4; frequencyStep = 1000;    tuneStepDisplay=(char*)" 1kHz"; break;
    case 4: tuneStepValue = 5; frequencyStep = 5000;    tuneStepDisplay=(char*)" 5kHz"; break;
    case 5: tuneStepValue = 6; frequencyStep = 10000;   tuneStepDisplay=(char*)"10kHz"; break;
    case 6: tuneStepValue = 1; frequencyStep = 1000000; tuneStepDisplay=(char*)" 1MHz"; break;
  }
}

/*****************************************************************************************/
/*! @brief  Set frequency to new frequency depending on the global variable bandSelector */
/*****************************************************************************************/
void setFrequencyPresets() {
  switch (bandSelectorValue)  {
    case 1: frequency = 100000;     bandSelectorTypeDisplay = (char*)"GEN" ; setSi5251Frequency(); interFrequency= 0; break;
    case 2: frequency = 800000;     bandSelectorTypeDisplay = (char*)"MW"  ; break;
    case 3: frequency = 1800000;    bandSelectorTypeDisplay = (char*)"160m";break;
    case 4: frequency = 3650000;    bandSelectorTypeDisplay = (char*)"80m" ;break;
    case 5: frequency = 4985000;    bandSelectorTypeDisplay = (char*)"60m" ;break;
    case 6: frequency = 6180000;    bandSelectorTypeDisplay = (char*)"49m" ;break;
    case 7: frequency = 7200000;    bandSelectorTypeDisplay = (char*)"40m" ;break;
    case 8: frequency = 10000000;   bandSelectorTypeDisplay = (char*)"31m" ;break;
    case 9: frequency = 11780000;   bandSelectorTypeDisplay = (char*)"25m" ;break;
    case 10: frequency = 13630000;  bandSelectorTypeDisplay = (char*)"22m" ;break;
    case 11: frequency = 14100000;  bandSelectorTypeDisplay = (char*)"20m" ;break;
    case 12: frequency = 15000000;  bandSelectorTypeDisplay = (char*)"19m" ;break;
    case 13: frequency = 17655000;  bandSelectorTypeDisplay = (char*)"16m" ;break;
    case 14: frequency = 21525000;  bandSelectorTypeDisplay = (char*)"13m" ;break;
    case 15: frequency = 27015000;  bandSelectorTypeDisplay = (char*)"11m" ;break;
    case 16: frequency = 28400000;  bandSelectorTypeDisplay = (char*)"10m" ;break;
    case 17: frequency = 50000000;  bandSelectorTypeDisplay = (char*)"6m" ;break;
    case 18: frequency = 100000000; bandSelectorTypeDisplay = (char*)"WFM" ;break;
    case 19: frequency = 130000000; bandSelectorTypeDisplay = (char*)"AIR" ;break;
    case 20: frequency = 144000000; bandSelectorTypeDisplay = (char*)"2m" ;break;
    case 21: frequency = 220000000; bandSelectorTypeDisplay = (char*)"1m" ;break;
  }
  if (frequency< 1000000) { 
    frequencyDisplay = (char*)"kHz";
  } else {
    frequencyDisplay = (char*)"MHz";
  }
  frequencyStep = 4; 
   setStep();
}

/**************************************************************************************/
/*! @brief  Set the frequentie formated on display                                    */
/**************************************************************************************/
void setDisplayFormatedFrequency() {
  unsigned int m = frequency / 1000000;
  unsigned int k = (frequency % 1000000) / 1000;
  unsigned int h = (frequency % 1000) / 1;

  display.clearDisplay();
  display.setTextSize(2);

  char buffer[15] = "";
  if (m < 1) {
    display.setCursor(41, 1); sprintf(buffer, "%003d.%003d", k, h);
  }
  else if (m < 100) {
    display.setCursor(5, 1); sprintf(buffer, "%2d.%003d.%003d", m, k, h);
  }
  else if (m >= 100) {
    unsigned int h = (frequency% 1000) / 10;
    display.setCursor(5, 1); sprintf(buffer, "%2d.%003d.%02d", m, k, h);
  }
  display.print(buffer);
}

/*****************************************************************************************/
/*! @brief  Set bandselector and bandpresets                                             */
/*****************************************************************************************/
void setBandSelector() {
  bandSelectorValue++;
  if (bandSelectorValue > 21) bandSelectorValue = 1;
  setFrequencyPresets();
  delay(50);
}


/**********************************************************************/
/*! @brief  Calculateremap values for the tunePointer and draw
            - Tune Pointer
            - SignalMeter
*/
/**********************************************************************/
void setDisplayBargraph() {
  byte y = map(tunePointer, 1, 42, 1, 14);
  display.setTextSize(1);
  
  // Pointer
  display.setCursor(0, 48);
  display.print("TU");
  display.fillRect(10 + (5 * y), 48, 2, 6, WHITE);

  // Bargraph
  display.setCursor(0, 57);
  display.print("SM");
  for (int x = 1; x < signalMeterRemap + 1; x++) {
    display.fillRect(10 + (5 * x), 58, 2, 6, WHITE);
  }
}

/**********************************************************************/
/*! @brief  Set the layout of the display                             */
/**********************************************************************/
void setDisplayTemplate() {
  display.setTextColor(WHITE);
  display.drawLine(0, 20, 127, 20, WHITE);
  display.drawLine(0, 43, 127, 43, WHITE);
  display.drawLine(105, 24, 105, 39, WHITE);
  display.drawLine(87, 24, 87, 39, WHITE);
  display.drawLine(87, 48, 87, 63, WHITE);
  display.drawLine(15, 55, 82, 55, WHITE);
  display.setTextSize(1);
  display.setCursor(59, 23);
  display.print("STEP");
  display.setCursor(54, 33);
  display.print(tuneStepDisplay);
  display.setTextSize(1);
  display.setCursor(92, 48);
  display.print("IF:");
  display.setCursor(92, 57);
  display.print(interFrequency);
  display.print("k");
  display.setTextSize(1);
  display.setCursor(110, 23);
  display.print(frequencyDisplay);
  display.setCursor(110, 33);
  display.print(interFrequencyTypeDisplay);
  display.setCursor(91, 28);
  display.print(rxtxDisplay);
  display.setTextSize(2);
  display.setCursor(0, 25);
  display.print(bandSelectorTypeDisplay); 
  setDisplayBargraph();
  display.display();
}


/*******************************************************************************************************/
/*! @brief  Read ADC set the value to 8 bits var signalMeterRemap and bound the value between 0 and 14 */
/*******************************************************************************************************/
void readSignalMeterADC() {
  signalMeter = analogRead(adc);
  signalMeterRemap = map(signalMeter, 0, S_GAIN, 1, 14); 
  if (signalMeterRemap > 14) {
    signalMeterRemap = 14;
  }  
}



/**********************************************************************/
/*! @brief  set the Initial text on display                           */
/**********************************************************************/
void setDisplayStartupText() {
  display.setTextSize(1); display.setCursor(13, 18);
  display.print("Si5351 VFO/RF GEN");
  display.setCursor(6, 40);
  display.print("JCR RADIO - Ver 2.0");
  display.display(); delay(2000);
}

/******************************************************************************/
/*! @brief  inital ssd1305
            set the initial input
            set start text
            inital the si5351
            enabled the interrupts
            set band preset
            set tunesteps
            set steps
*/
/******************************************************************************/
void setup() {
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.display();

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(tunestep, INPUT_PULLUP);
  pinMode(band, INPUT_PULLUP);
  pinMode(rx_tx, INPUT_PULLUP);

  //setDisplayStartupText();  //If you hang on startup, comment

  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
  si5351.set_correction(calibrationFactor, SI5351_PLL_INPUT_XO);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
  si5351.output_enable(SI5351_CLK0, 1);                  //1 - Enable / 0 - Disable CLK
  si5351.output_enable(SI5351_CLK1, 0);
  si5351.output_enable(SI5351_CLK2, 0);

  PCICR |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);
  sei();
  setFrequencyPresets();
}

/******************************************************************************/
/*! @brief  Check if frequency or interFrequency changed                      */
/******************************************************************************/
void checkSi5251Changes(){
    if (frequencyPrevious != frequency ||interFrequencyPrevious != interFrequency  ) {
      setSi5251Frequency();
      frequencyPrevious = frequency;
      interFrequencyPrevious = interFrequency;

      if (interFrequency== 0) { 
          interFrequencyTypeDisplay=(char*)"VFO";
      } else {
         interFrequencyTypeDisplay=(char*)"L O";
      }
  }

}

/******************************************************************************/
/*! @brief  if button touch or rotary turn new frequencies etc will be set    */
/******************************************************************************/
void loop() {
  timeNow = millis();
  
  checkSi5251Changes();
   
  if (signalMeterRemapPrevious != signalMeterRemap) {
    signalMeterRemapPrevious = signalMeterRemap;
  }

  if (digitalRead(tunestep) == LOW) {
    setStep();
    delay(300);
  }

  if (digitalRead(band) == LOW) {
    setBandSelector();
    delay(300);
  }

  if (digitalRead(rx_tx) == LOW) {
      rxtxSwitch = true;
      rxtxDisplay = (char*)"TX";
      interFrequency= 0;
  } else { 
      rxtxSwitch = false;
      rxtxDisplay = (char*)"RX";
      interFrequency= IF;
  }

   if ((timeNow + period) > millis()) {
     setDisplayFormatedFrequency();
     setDisplayTemplate();
  }
  readSignalMeterADC();
}
//EOF
