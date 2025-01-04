
#include <avr/pgmspace.h>          // needed for to story large array's
#include <Adafruit_GFX.h>          // Adafruit GFX graphics core library, this is the 'core' class that all our other graphics libraries derive from. https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_SSD1306.h>      // SSD1306 oled driver library for monochrome 128x64 and 128x32 displays https://github.com/adafruit/Adafruit_SSD1306


const char   display_step_0[] PROGMEM = "     ";
const char   display_step_1[] PROGMEM = "  1Hz";
const char   display_step_2[] PROGMEM = " 10Hz";
const char   display_step_3[] PROGMEM = " 1kHz";
const char   display_step_4[] PROGMEM = " 5kHz";
const char   display_step_5[] PROGMEM = "10kHz";
const char   display_step_6[] PROGMEM = " 1MHz";
const char * const  PROGMEM tuneStepsDisplay[] = {display_step_0,display_step_1,display_step_2,display_step_3,display_step_4,display_step_5,display_step_6};  // the displaysteps 


const char   display_band_00[] PROGMEM = "     ";
const char   display_band_01[] PROGMEM = "GEN  ";
const char   display_band_02[] PROGMEM = "MW   ";
const char   display_band_03[] PROGMEM = "160m ";
const char   display_band_04[] PROGMEM = "80m  ";
const char   display_band_05[] PROGMEM = "60m  ";
const char   display_band_06[] PROGMEM = "49m  ";
const char   display_band_07[] PROGMEM = "40m  "; 
const char   display_band_08[] PROGMEM = "31m  ";
const char   display_band_09[] PROGMEM = "25m  ";
const char   display_band_10[] PROGMEM = "22m  ";    
const char   display_band_11[] PROGMEM = "20m  ";
const char   display_band_12[] PROGMEM = "19m  ";
const char   display_band_13[] PROGMEM = "16m  ";
const char   display_band_14[] PROGMEM = "13m  ";
const char   display_band_15[] PROGMEM = "11m  ";
const char   display_band_16[] PROGMEM = "10m  ";
const char   display_band_17[] PROGMEM = "6m   "; 
const char   display_band_18[] PROGMEM = "WFM  ";
const char   display_band_19[] PROGMEM = "AIR  ";
const char   display_band_20[] PROGMEM = "2m   ";    
const char   display_band_21[] PROGMEM = "1m   ";
const char * const  PROGMEM displayBand[] = {
                                                display_band_00,display_band_01,display_band_02,display_band_03,display_band_04,
                                                display_band_05,display_band_06,display_band_07,display_band_08,display_band_09,
                                                display_band_10,display_band_11,display_band_12,display_band_13,display_band_14,
                                                display_band_15,display_band_16,display_band_17,display_band_18,display_band_19,
                                                display_band_20,display_band_21
                                                };

namespace MyDisplay{
 
     class Screen {
          
          public:
          
            /***************************************************************************************/
            /*! @brief  Initial setup op the display                                               */
            /***************************************************************************************/
             void init();
           
            /***************************************************************************************/
            /*! @brief  Initial timesetup                                                          */
            /***************************************************************************************/
             void setTimeNow();
           
            /***************************************************************************************/
            /*! @brief  Start text to screen to display                                            */
            /***************************************************************************************/
             void setStartupText();
           
            /***************************************************************************************/
            /*! @brief  Start text to screen to display           
                @param  tuneStepValue          Value to set the frequency steps 
                @param  interFrequency         The default interFrequency ex: 455 = 455kHz, 10700 = 10.7MHz,  0 = to direct convert receiver or RF generator, + will add and - will subtract IF  offfset. 
                @param  frequency              Working frequency
                @param  bandSelectorValue      Default set by BAND_INIT otherwise set by the  band selector push button. 
                @param  rxtxSwitch             Transmitter/Receiver     
                @param  tunePointer            Set the pointer for the tuning display
                @param  signalMeterRemap       The signal meter remap 
            */
            /***************************************************************************************/
            void setScreen(
                            byte tuneStepValue, 
                            int interFrequency, 
                            unsigned long frequency,
                            byte bandSelectorValue,
                            bool rxtxSwitch, 
                            byte tunePointer, 
                            byte signalMeterRemap);

           private:
           
            unsigned long timeNow ;             // The current time
            unsigned int period ;               // Used for calculation the performance time .
            char  bandSelectorTypeDisplay[6];   // The band name for the Display. ex: GEN, MW, 160m 80m etc. 
            char  tuneStepDisplay[7]; 
           
            /***************************************************************************************/
            /*! @brief  setup the static Screen elements                                          */
            /***************************************************************************************/
            void setScreenStatic();
           
            /***************************************************************************************/
            /*! @brief  Set the type to kHz or MHz for the display    
                @param  freq The currenct frequency 
                @return kHz or MHz 
            */
            /***************************************************************************************/
            char* setFrequencyScreen(long freq);
           
            /***************************************************************************************/
            /*! @brief  Set the interfrequentye for the display     
                @param  freq The currenct frequency 
                @return VFO or L O 
            */
            /***************************************************************************************/
            char* setInterFrequentieScreen(long freq);

            /***************************************************************************************/
            /*! @brief  Set on display TX OR RX    
                @param  rxtxSwitch true or fals 
                @return TX or RX 
            */
            /***************************************************************************************/
            char* setRxTxScreen(bool rxtxSwitch);

            /***************************************************************************************/
            /*! @brief  Set on display the recalculated tunepoint    
                @param  tunePointer      The tune pointer 
                @param singalMeterRemap  The recalculated tune pointer
            */
            /***************************************************************************************/
            void setBargraph(byte tunePointer, byte signalMeterRemap) ;

            /***************************************************************************************/
            /*! @brief  Set the format of the frequency value on display     
                @param  frequency        The current frequency 
            */
            /***************************************************************************************/
            void setFormattedFrequency(unsigned long frequency);
   
     };
     extern Screen displayManager;

}

