#define tunestep   A0        //The pin used by tune step push button.
#define band       A1        //The pin used by band selector push button.
#define rx_tx      A2        //The pin used by RX / TX selector switch, RX = switch open, TX = switch closed to GND. When in TX, the IF value is not considered.
#define adc        A3        //The pin used by Signal Meter A/D input.
     

/************************************************* */
/*! @brief Initialize TunestepButton               */
/************************************************* */
void initButtonTuneStep() {
     pinMode(tunestep, INPUT_PULLUP);
}   

/************************************************* */
/*! @brief Initialize Band Selector Button         */
/************************************************* */
void initButtonBandSelector() {
     pinMode(band, INPUT_PULLUP);
}    

/************************************************* */
/*! @brief Initialize Rx Tx Button                 */
/************************************************* */
void initButtonRxTx() {
      pinMode(rx_tx, INPUT_PULLUP);
}   

/************************************************************************** */
/*! @brief if TunestepButton pressed  set tunestepvalue and frequencystep   */
/************************************************************************** */
void checkButtonTuneStep() {
      if (digitalRead(tunestep) == LOW) {
            MyData::dataManager.setByButtonTuneStep();
            delay(300);
      }       
}

/************************************************************************** */
/*! @brief if Bandselector pressed  set frequency  and   bandselector value
           if bandselector start again tuneStepValue is set to default      */
/************************************************************************** */
void checkButtonBandSelector() {
     if (digitalRead(band) == LOW) {
         MyData::dataManager.setByButtonBandSelector();
         delay(300);
      }
}

/**************************************************************************** */
/*! @brief if RxTx button  pressed  set rxtxSwitch  and  interFrequency value */
/**************************************************************************** */
void checkButtonRxTx() {
    if (digitalRead(rx_tx) == LOW) {
         MyData::dataManager.setByButtonRxTxSwitch(true,0);
    } else { 
         MyData::dataManager.setByButtonRxTxSwitch(false,IF);
    }
}

/**************************************************************************** */
/*! @brief read the analog signal for the signal meter                        */
/**************************************************************************** */
void readSignalMeterADC() {
        MyData::dataManager.setByAnalogRead(analogRead(adc));
}   
