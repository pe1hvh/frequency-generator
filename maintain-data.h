#define IF         455             // Enter your IF frequency, ex: 455 = 455kHz, 10700 = 10.7MHz, 0 = to direct convert receiver or RF generator, + will add and - will subtract IF offfset.


namespace MyData {

    const unsigned long frequencySteps[7]   PROGMEM = {0UL,1UL,10UL,1000UL,5000UL,10000UL,1000000UL};     // the frequencySteps corrersponding by the steps    
    const unsigned long frequencyBanden[22] PROGMEM = {0, 100000, 800000, 1800000, 3650000, 4985000,      // the frequency bands
                                                       6180000, 7200000, 10000000, 11780000, 13630000,
                                                       14100000, 15000000, 17655000, 21525000, 27015000,
                                                       28400000, 50000000, 100000000, 130000000,
                                                       144000000, 220000000}; 
    class Data {
        
        private:

            // variable used by the getters 
            unsigned long frequency     = 7200000;   // The frequency for setting the correct clock frequency possible values ex:  100000,   800000,  1800000,  3650000,  4985000 etc
            int   interFrequency        = IF;        // The default interFrequency ex: 455 = 455kHz, 10700 = 10.7MHz,  0 = to direct convert receiver or RF generator, + will add and - will subtract IF  offfset.
            bool  rxtxSwitch            = false;     // Rx Tx Switch
            byte  tuneStepValue         = 4;         // The default value otherwise set by the TuneStepButton
            byte  bandSelectorValue     = 7;         // Default 7 otherwise set by the  band selector button. /Enter your initial Band (1-21) at startup, ex: 1 = frequencyGenerator, 2 = 800kHz (MW), 7 = 7.2MHz (40m), 11 = 14.1MHz (20m).
            byte  tunePointer           = 0;         // Default set to left otherwise set by rotary
            uint8_t signalMeterRemap ;               // Remapped and bounded value
            // end variable used by the getters

            unsigned long frequencyStep = 10;               //The frequency step ex: 1, 10, 1000, 5000, 10000 set by tunestepvalue
            byte  tuneStepValues[7]     = {0,2,3,4,5,6,1};  //The next tunestepValues set by tuneStepValue
            int adc;                                        //ADC pin number
            const int signalGain        = 303;              //Maximum ADC value
            int signalMeter;                                // Raw ADC reading
            

        public:

            /********************************************************** */
            /*! @brief the public getter for frequency 
                @return frequency */
            /********************************************************** */
            unsigned long getFrequency() {
                return frequency;
            }

            /********************************************************** */
            /*! @brief the public getter for frequencyStep 
                @return frequencyStep */
            /********************************************************** */            
            unsigned long getFrequencyStep() {
                return frequencyStep;
            }

            /********************************************************** */
            /*! @brief the public getter for interFrequency 
                @return interFrequency */
            /********************************************************** */
            int getInterFrequency() {
                return interFrequency;
            }

            /********************************************************** */
            /*! @brief the public getter for rxtxSwitch 
                @return rxtxSwitch */
            /********************************************************** */
            bool getRxTxSwitch() {
                return rxtxSwitch;
            }

            /********************************************************** */
            /*! @brief the public getter for tuneStepValue 
                @return tuneStepValue */
            /********************************************************** */
            byte getTuneStepValue(){
                return tuneStepValue;
            }

            /********************************************************** */
            /*! @brief the public getter for bandSelectorValue 
                @return bandSelectorValue */
            /********************************************************** */
            byte getBandSelector() {
                return bandSelectorValue;
            }

            /********************************************************** */
            /*! @brief the public getter for tunePointer 
                @return tunePointer */
            /********************************************************** */
            byte getTunePointer() {
                return tunePointer;
            }

            /********************************************************** */
            /*! @brief the public getter for signalMeterRemap 
                @return signalMeterRemap */
            /********************************************************** */
            uint8_t getSignalMeterRemap() const {
                return signalMeterRemap;
            }

            /********************************************************** */
            /*! @brief the public setter for the rotaryhandler 
                @param direction DIR_CW = clockwise  tunePointer  
                                 DIR_CCW= counter clockwise */
            /********************************************************** */
            void setByRotary(char direction){
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
            void setByButtonTuneStep() {
                tuneStepValue = tuneStepValues[tuneStepValue];
                frequencyStep = pgm_read_dword_near(&frequencySteps[tuneStepValue]);  
            }   

            /********************************************************** */
            /*! @brief the public setter for Button BandSelector        */
            /********************************************************** */
            void setByButtonBandSelector() {
                bandSelectorValue++;
                if (bandSelectorValue > 21) {bandSelectorValue = 1; tuneStepValue = 4; }
                frequency   = pgm_read_dword_near(&frequencyBanden[bandSelectorValue]);
                tunePointer = 0;
            }    

            /********************************************************** */
            /*! @brief the public setter for Button Rx or TX         */
            /********************************************************** */
            void setByButtonRxTxSwitch(bool pswitch, long interF) {
                rxtxSwitch = pswitch;
                interFrequency= interF;
            }    

            /********************************************************** */
            /*! @brief the public setter for analogread  
                @param singalMeter the raw data from the analogread     */
            /********************************************************** */
            void setByAnalogRead(uint8_t signalMeter){
                signalMeterRemap = map(signalMeter, 0, signalGain, 1, 14);
                if (signalMeterRemap > 14) {
                    signalMeterRemap = 14;
                }
            }
      };
      
    Data dataManager;
}