

namespace MyData {

            /********************************************************** */
            /*! @brief setting the initals                               */
            /********************************************************** */
            void Data::init() {
  
                 frequency          = 7200000;   // The frequency for setting the correct clock frequency possible values ex:  100000,   800000,  1800000,  3650000,  4985000 etc
                 interFrequency     = IF;        // The default interFrequency ex: 455 = 455kHz, 10700 = 10.7MHz,  0 = to direct convert receiver or RF generator, + will add and - will subtract IF  offfset.
                 rxtxSwitch         = false;     // Rx Tx Switch
                 tuneStepValue      = 4;         // The default value otherwise set by the TuneStepButton
                 bandSelectorValue  = 7;         // Default 7 otherwise set by the  band selector button. /Enter your initial Band (1-21) at startup, ex: 1 = frequencyGenerator, 2 = 800kHz (MW), 7 = 7.2MHz (40m), 11 = 14.1MHz (20m).
                 tunePointer        = 0;         // Default set to left otherwise set by rotary
                 frequencyStep      = 10;               //The frequency step ex: 1, 10, 1000, 5000, 10000 set by tunestepvalue
            }

  
            /********************************************************** */
            /*! @brief the public getter for frequency 
                @return frequency */
            /********************************************************** */
            unsigned long Data::getFrequency() {
                return frequency;
            }

            /********************************************************** */
            /*! @brief the public getter for frequencyStep 
                @return frequencyStep */
            /********************************************************** */            
            unsigned long Data::getFrequencyStep() {
                return frequencyStep;
            }

            /********************************************************** */
            /*! @brief the public getter for interFrequency 
                @return interFrequency */
            /********************************************************** */
            int Data::getInterFrequency() {
                return interFrequency;
            }

            /********************************************************** */
            /*! @brief the public getter for rxtxSwitch 
                @return rxtxSwitch */
            /********************************************************** */
            bool Data::getRxTxSwitch() {
                return rxtxSwitch;
            }

            /********************************************************** */
            /*! @brief the public getter for tuneStepValue 
                @return tuneStepValue */
            /********************************************************** */
            uint8_t Data::getTuneStepValue(){
                return tuneStepValue;
            }

            /********************************************************** */
            /*! @brief the public getter for bandSelectorValue 
                @return bandSelectorValue */
            /********************************************************** */
            uint8_t Data::getBandSelector() {
                return bandSelectorValue;
            }

            /********************************************************** */
            /*! @brief the public getter for tunePointer 
                @return tunePointer */
            /********************************************************** */
            uint8_t Data::getTunePointer() {
                return tunePointer;
            }

            /********************************************************** */
            /*! @brief the public getter for signalMeterRemap 
                @return signalMeterRemap */
            /********************************************************** */
            uint8_t Data::getSignalMeterRemap() {
                return signalMeterRemap;
            }

            /********************************************************** */
            /*! @brief the public setter for the rotaryhandler 
                @param direction DIR_CW = clockwise  tunePointer  
                                 DIR_CCW= counter clockwise */
            /********************************************************** */
            void Data::setByRotary(char direction){
                if (direction == DIR_CW) {
                          frequency = min(frequency + frequencyStep, 225000000);
                          tunePointer = (tunePointer % 42) + 1;
                } else if (direction == DIR_CCW) {
                          frequency = max(frequency - frequencyStep, 10000);
                          tunePointer = (tunePointer > 1) ? tunePointer - 1 : 42;
                }

                //Frequency can not by smaller than the frequencyStep 
                if (frequencyStep == 1000000 && frequency <= 1000000)  {              
                          frequency= 1000000; 
                } 
            }

            /********************************************************** */
            /*! @brief the public setter for Button TuneStepvhandler    */
            /********************************************************** */
            void Data::setByButtonTuneStep(bool tunestep) {
                if(tunestep==LOW) {
                    tuneStepValue = tuneStepValues[tuneStepValue];
                    frequencyStep = pgm_read_dword_near(&frequencySteps[tuneStepValue]);  
                    delay(300);
                }
            }   

            /********************************************************** */
            /*! @brief the public setter for Button BandSelector        */
            /********************************************************** */
            void Data::setByButtonBandSelector(bool band) {
                if(band == LOW){
                    bandSelectorValue++;
                    if (bandSelectorValue > 21) {bandSelectorValue = 1; tuneStepValue = 4; }
                    frequency   = pgm_read_dword_near(&frequencyBanden[bandSelectorValue]);
                    tunePointer = 0;
                    delay(300);
                }
            }    

            /********************************************************** */
            /*! @brief the public setter for Button Rx or TX         */
            /********************************************************** */
            void Data::setByButtonRxTxSwitch(bool rx_tx) {
                if(rx_tx == LOW) {
                    rxtxSwitch     = true;
                    interFrequency = 0;
                } else { 
                    rxtxSwitch     = false;
                    interFrequency = IF;
                }
            }    

            /********************************************************** */
            /*! @brief the public setter for analogread  
                @param singalMeter the raw data from the analogread     */
            /********************************************************** */
            void Data::setByAnalogRead(uint8_t signalMeter){
                signalMeterRemap = map(signalMeter, 0, signalGain, 1, 14);
                if (signalMeterRemap > 14) {
                    signalMeterRemap = 14;
                }
            }
           
    Data dataManager;
}
