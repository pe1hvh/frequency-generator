
#include <si5351.h>                // A full-featured library for the Si5351 series of clock generator ICs from Silicon Labs  https://github.com/etherkit/Si5351Arduino


namespace MySi5251 {
   
    class Si5251Handler {
 
        private:

            long calibrationFactor         ;    //  Si5351 calibration factor, adjust to get exatcly 10MHz. Increasing this value will decreases the frequency and vice versa.
            unsigned long frequencyPrevious;    // The previous frequency
            long  interFrequencyPrevious   ;    // The previous interFrequency
        
        public:

            /**************************************************************************************/
            /*! @brief  Set the inital values for the si5351                                      */
            /**************************************************************************************/
            void init();

            /**************************************************************************************/
            /*! @brief  Set the frequenty for clock0 using 
                @param  freq the current frequencyd 
                @param  interFreq the current inter frequencyy 
            */
            /**************************************************************************************/
            void setSi5251Frequency(unsigned long freq, long  interFreq);

            /******************************************************************************/
            /*! @brief  Check if frequency or interFrequency changed  
                @param  freq the current frequencyd 
                @param  interFreq the current inter frequencyy                    
            */
            /******************************************************************************/
            void checkSi5251Changes(unsigned long freq, long  interFreq);
    
    };

 extern Si5251Handler si5251Manager;
}


