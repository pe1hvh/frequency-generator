/***************************************************************************************/
/* Libraries                                                                           */
/***************************************************************************************/
#include <Rotary.h>                // Arduino library for reading rotary directions that output a 2-bit gray code. Ben  Buxton https://github.com/brianlow/Rotary
#include <si5351.h>                // A full-featured library for the Si5351 series of clock generator ICs from Silicon Labs  https://github.com/etherkit/Si5351Arduino
#include <avr/pgmspace.h>          // needed for to story large array's

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
Si5351 si5351(0x60);                                         // Initialize Si5351 object with I2C Address 0x60 (Standard IC2 bus adres for Si5251.
/***************************************************************************************/
/* Global Variabelen                                                                   */
/***************************************************************************************/
unsigned long frequency;           // Global: The frequency for setting the correct clock frequency possible values ex:  100000,   800000,  1800000,  3650000,  4985000 etc
unsigned long frequencyPrevious;   // Global: The previous frequency
unsigned long frequencyStep;       // Global: The frequency step ex: 1, 10, 1000, 5000, 10000

unsigned long timeNow = 0;          // Global: The current time
unsigned int period = 100;          // Global: Used for calculation the performance time .

long  interFrequency = IF;          // Global: The default interFrequency ex: 455 = 455kHz, 10700 = 10.7MHz,  0 = to direct convert receiver or RF generator, + will add and - will subtract IF  offfset.
long  interFrequencyPrevious = 0;   // Global: The previous interFrequency

unsigned int signalMeter;           // Global: Value set by the pin for Signal Meter A/D input.
byte  signalMeterRemap;             // Global: The re-map value, set in readSignal().
byte  signalMeterRemapPrevious;     // Global: The previous re-map value, set in readSignal().
byte  tuneStepValue=4;              // Global: Value set by the tune step push button
byte  tunePointer = 1;              // Global: Up/Down graph tune pointer, set by set_frequency()
byte  bandSelectorValue = BAND_INIT;// Global: Default set by BAND_INIT otherwise set by the  band selector push button.
bool  rxtxSwitch = false;           // Global: The status of the  RX / TX selector switch

long calibrationFactor = XT_CAL_F;  // Global: Si5351 calibration factor, adjust to get exatcly 10MHz. Increasing this value will decreases the frequency and vice versa.

// Used by the function setSteps()
byte                     tuneStepValues[7]  = {0,2,3,4,5,6,1};                                    // the tunestepValue
const unsigned long      frequencySteps[7]  PROGMEM =  {0UL,1UL,10UL,1000UL,5000UL,10000UL,1000000UL};     // the frequencySteps corrersponding by the steps  


// Used by the function setFrequencyPresets
const unsigned long frequencyBanden[22] PROGMEM = {0, 100000, 800000, 1800000, 3650000, 4985000,
                                                   6180000, 7200000, 10000000, 11780000, 13630000,
                                                   14100000, 15000000, 17655000, 21525000, 27015000,
                                                   28400000, 50000000, 100000000, 130000000,
                                                   144000000, 220000000}; 
