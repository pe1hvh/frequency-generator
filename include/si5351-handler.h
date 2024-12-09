#include <Arduino.h>
#include <si5351.h>                // A full-featured library for the Si5351 series of clock generator ICs from Silicon Labs  https://github.com/etherkit/Si5351Arduino

Si5351 si5351(0x60);               // Initialize Si5351 object with I2C Address 0x60 (Standard IC2 bus adres for Si5251. 

namespace MySi5251 {



class Si5251Handler {
 
    private:
 
        const long calibrationFactor = 33000;     //  Si5351 calibration factor, adjust to get exatcly 10MHz. Increasing this value will decreases the frequency and vice versa.
        unsigned long frequencyPrevious;          // The previous frequency
        long  interFrequencyPrevious = 0;         // The previous interFrequency
    
    public:

        /**************************************************************************************/
        /*! @brief  Set the inital values for the si5351                                      */
        /**************************************************************************************/
        void init() {

            si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
            si5351.set_correction(calibrationFactor, SI5351_PLL_INPUT_XO);
            si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
            si5351.output_enable(SI5351_CLK0, 1);                  //1 - Enable / 0 - Disable CLK
            si5351.output_enable(SI5351_CLK1, 0);
            si5351.output_enable(SI5351_CLK2, 0);

        }   

        /**************************************************************************************/
        /*! @brief  Set the frequenty for clock0 using 
            @param  freq the current frequencyd 
            @param  interFreq the current inter frequencyy 
        */
        /**************************************************************************************/
        void setSi5251Frequency(unsigned long freq, long  interFreq)  {     // Global: The frequency for setting the correct clock frequency possible values ex:  100000,   800000,  1800000,  3650000,  4985000 etc
            si5351.set_freq((freq + (interFreq* 1000ULL)) * 100ULL, SI5351_CLK0);
        }

        /******************************************************************************/
        /*! @brief  Check if frequency or interFrequency changed  
            @param  freq the current frequencyd 
            @param  interFreq the current inter frequencyy                    
        */
        /******************************************************************************/
        void checkSi5251Changes(unsigned long freq, long  interFreq){
            if (frequencyPrevious != freq || interFrequencyPrevious != interFreq ) {
                setSi5251Frequency(freq,interFreq);
                frequencyPrevious      = freq;
                interFrequencyPrevious = interFreq;
            }
        }
    
};

Si5251Handler si5251Manager;
}


