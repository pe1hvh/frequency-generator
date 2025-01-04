#include <avr/pgmspace.h>          // needed for to story large array's
#include <Rotary.h>                // 
#define IF         455             // Enter your IF frequency, ex: 455 = 455kHz, 10700 = 10.7MHz, 0 = to direct convert receiver or RF generator, + will add and - will subtract IF offfset.


const unsigned long frequencySteps[7]   PROGMEM = {0UL,1UL,10UL,1000UL,5000UL,10000UL,1000000UL};     // the frequencySteps corrersponding by the steps    
const unsigned long frequencyBanden[22] PROGMEM = {0, 100000, 800000, 1800000, 3650000, 4985000,      // the frequency bands
                                                   6180000, 7200000, 10000000, 11780000, 13630000,
                                                   14100000, 15000000, 17655000, 21525000, 27015000,
                                                   28400000, 50000000, 100000000, 130000000,
                                                   144000000, 220000000}; 

const int signalGain        = 303;              //Maximum ADC value

namespace MyData {

    class Data {
        
        private:

            // variable used by the getters 
            unsigned long frequency     ;   // The frequency for setting the correct clock frequency possible values ex:  100000,   800000,  1800000,  3650000,  4985000 etc
            int   interFrequency        ;   // The default interFrequency ex: 455 = 455kHz, 10700 = 10.7MHz,  0 = to direct convert receiver or RF generator, + will add and - will subtract IF  offfset.
            bool  rxtxSwitch            ;   // Rx Tx Switch
            uint8_t  tuneStepValue      ;   // The default value otherwise set by the TuneStepButton
            uint8_t  bandSelectorValue  ;   // Default 7 otherwise set by the  band selector button. /Enter your initial Band (1-21) at startup, ex: 1 = frequencyGenerator, 2 = 800kHz (MW), 7 = 7.2MHz (40m), 11 = 14.1MHz (20m).
            uint8_t  tunePointer        ;   // Default set to left otherwise set by rotary
            uint8_t signalMeterRemap    ;   // Remapped and bounded value
            // end variable used by the getters

            unsigned long frequencyStep ;    //The frequency step ex: 1, 10, 1000, 5000, 10000 set by tunestepvalue
            int adc;                         //ADC pin number
            int signalMeter;                 // Raw ADC reading
            uint8_t  tuneStepValues[7]  = {0, 2, 3, 4, 5, 6, 1};  //The next tunestepValues set by tuneStepValue

        public:

            /********************************************************** */
            /*! @brief setting the initials  */
            /********************************************************** */
            void init();

            /********************************************************** */
            /*! @brief the public getter for frequency 
                @return frequency */
            /********************************************************** */
            unsigned long getFrequency();

            /********************************************************** */
            /*! @brief the public getter for frequencyStep 
                @return frequencyStep */
            /********************************************************** */            
            unsigned long getFrequencyStep();

            /********************************************************** */
            /*! @brief the public getter for interFrequency 
                @return interFrequency */
            /********************************************************** */
            int getInterFrequency();

            /********************************************************** */
            /*! @brief the public getter for rxtxSwitch 
                @return rxtxSwitch */
            /********************************************************** */
            bool getRxTxSwitch();

            /********************************************************** */
            /*! @brief the public getter for tuneStepValue 
                @return tuneStepValue */
            /********************************************************** */
            uint8_t getTuneStepValue();

            /********************************************************** */
            /*! @brief the public getter for bandSelectorValue 
                @return bandSelectorValue */
            /********************************************************** */
            uint8_t getBandSelector();

            /********************************************************** */
            /*! @brief the public getter for tunePointer 
                @return tunePointer */
            /********************************************************** */
            uint8_t getTunePointer();

            /********************************************************** */
            /*! @brief the public getter for signalMeterRemap 
                @return signalMeterRemap */
            /********************************************************** */
            uint8_t getSignalMeterRemap();

            /********************************************************** */
            /*! @brief the public setter for the rotaryhandler 
                @param direction DIR_CW = clockwise  tunePointer  
                                 DIR_CCW= counter clockwise */
            /********************************************************** */
            void setByRotary(char direction);

            /********************************************************** */
            /*! @brief the public setter for Button TuneStepvhandler    */
            /********************************************************** */
            void setByButtonTuneStep(bool tunestep);

            /********************************************************** */
            /*! @brief the public setter for Button BandSelector        */
            /********************************************************** */
            void setByButtonBandSelector(bool band);

            /********************************************************** */
            /*! @brief the public setter for Button Rx or TX         */
            /********************************************************** */
            void setByButtonRxTxSwitch(bool rx_tx);

            /********************************************************** */
            /*! @brief the public setter for analogread  
                @param singalMeter the raw data from the analogread     */
            /********************************************************** */
            void setByAnalogRead(uint8_t signalMeter);
      };
      
    extern Data dataManager;
}
