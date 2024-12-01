/**********************************************************************************************************
  10kHz to 225MHz VFO / RF Generator with Si5351 and Arduino Nano, with Intermediate
  Frequency (IF) offset (+ or -), RX/TX Selector for QRP Transceivers, Band Presets
  and Bargraph S-Meter.  Based on source made by J. CesarSound - ver 2.0 - Feb/2021.
  
  Forked by JA van Hernen
   - re-code: standard lowerCamelCase nameconvention
   - standard indentation
   - standard formating
   - cleancode (so far as possible)
   - comment code
  Version 3.0 december 2024
   - put all the declarations in a header file
   - add lookup array's to removed the multiple case statements.  
   - removed some unnessary ifs
   - building a Display class 
***********************************************************************************************************/
#include "frequentie-generator.h"  //Declarations of libraries,globals,array's etc 
#include "display.h"               //Display class   

/***************************************************************************************/
/*! @brief  Set the frequency started by de interupt handler.
    @param  direction given direction of the rotarysteps:
      DIR_CW  = clockwise step
      DIR_CWW = bandSelectorValueer clockwise step
*/
/*************************************************************************************/
void setFrequency(char direction) {
    if (direction == DIR_CW) {         
        //DIR_CW Direction of rotary steps is clock wise
        frequency = frequency + frequencyStep;                
        if (frequency >= 225000000) { frequency = 225000000; }        
        tunePointer = tunePointer + 1;
        if (tunePointer > 42) { tunePointer = 1; }
    } else {                                                              
        //(DIR_CCW:  Direction of rotery steps is counter clock wise   
        frequency= frequency- frequencyStep;    
        if(frequency < 10000)  { frequency = 10000; }           
        tunePointer = tunePointer - 1;
        if (tunePointer < 1)  {tunePointer = 42; }
    } 
    //Frequency can not by smaller than the frequencyStep 
    if (frequencyStep == 1000000 && frequency <= 1000000)  {              
        frequency= 1000000; 
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
  setFrequency(result);
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
  frequencyStep = pgm_read_byte(&frequencySteps[tuneStepValue]);
  tuneStepValue = tuneStepValues[tuneStepValue];
}

/*****************************************************************************************/
/*! @brief  Set frequency to new frequency depending on the global variable bandSelector */
/*****************************************************************************************/
void setFrequencyPresets() {
  if(bandSelectorValue==1) {setSi5251Frequency(); interFrequency= 0;}
  frequency = pgm_read_dword_near(frequencyBanden + bandSelectorValue);
  tuneStepValue = 4; 
  setStep();
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

/******************************************************************************/
/*! @brief  Check if frequency or interFrequency changed                      */
/******************************************************************************/
void checkSi5251Changes(){
    if (frequencyPrevious != frequency || interFrequencyPrevious != interFrequency  ) {
      setSi5251Frequency();
      frequencyPrevious = frequency;
      interFrequencyPrevious = interFrequency;
   }

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
  MyDisplay::displayManager.init();
  MyDisplay::displayManager.showStartupText(); //If you hang on startup, comment

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(tunestep, INPUT_PULLUP);
  pinMode(band, INPUT_PULLUP);
  pinMode(rx_tx, INPUT_PULLUP);

  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
  si5351.set_correction(calibrationFactor, SI5351_PLL_INPUT_XO);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
  si5351.output_enable(SI5351_CLK0, 1);                  //1 - Enable / 0 - Disable CLK
  si5351.output_enable(SI5351_CLK1, 0);
  si5351.output_enable(SI5351_CLK2, 0);

  PCICR |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);
  sei();
  tuneStepValue = 4;
  setFrequencyPresets();
}

/***********************************************************************************/
/*! @brief  if the buttons touch or rotary turn new frequencies etc will be set    */
/***********************************************************************************/
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
      interFrequency= 0;
  } else { 
      rxtxSwitch = false;
      interFrequency= IF;
  }

  if ((timeNow + period) > millis()) {

        MyDisplay::displayManager.showTemplate(tuneStepValue, 
                                    interFrequency, 
                                    frequency,
                                    bandSelectorValue,
                                    rxtxSwitch, 
                                    tunePointer, 
                                    signalMeterRemap);
    }
  readSignalMeterADC();
}
//EOF
