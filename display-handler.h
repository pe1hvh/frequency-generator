#include <avr/pgmspace.h>          // needed for to story large array's
#include <Adafruit_GFX.h>          // Adafruit GFX graphics core library, this is the 'core' class that all our other graphics libraries derive from. https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_SSD1306.h>      // SSD1306 oled driver library for monochrome 128x64 and 128x32 displays https://github.com/adafruit/Adafruit_SSD1306

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire); // Initialize Adafruit_SSD1306 object (Type Adafruit_SSD1306) width 128 pixels hight 64 pixels, using the Wire object(library) by reference


namespace MyDisplay {

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


    class Display {

        private:

            unsigned long timeNow = 0;          // The current time
            unsigned int period = 100;          // Used for calculation the performance time .
            char  bandSelectorTypeDisplay[6];   // The band name for the Display. ex: GEN, MW, 160m 80m etc. 
            char  tuneStepDisplay[7];           // The frequency step ex: " 1MHz", "  1Hz", " 10Hz", " 1kHz" ," 5kHz" , "10kHz"
    
        public:
                
            /***************************************************************************************/
            /*! @brief  Initial setup op the display                                               */
            /***************************************************************************************/
            void init() {
                Wire.begin();
                display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
                display.clearDisplay();
                display.setTextColor(WHITE);
                display.display();
            }

            /***************************************************************************************/
            /*! @brief  Initial timesetup                                                          */
            /***************************************************************************************/
            void setTimeNow() {
                timeNow = millis();
            }
            
            /***************************************************************************************/
            /*! @brief  Start text to screen to display                                            */
            /***************************************************************************************/
            void setStartupText() {
                display.setTextSize(1);
                display.setCursor(13, 18);
                display.print("Si5351 VFO/RF GEN");
                display.setCursor(6, 40);
                display.print("JCR RADIO - Ver 3.0");
                display.display();
                delay(3000);
                display.clearDisplay();
        
            }
            
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
                            byte signalMeterRemap) {
                            
                if(timeNow + period > millis()) {
                    display.clearDisplay();
                
                    setScreenStatic();

                    setFormattedFrequency(frequency);   

                    strcpy_P(bandSelectorTypeDisplay, (PGM_P)pgm_read_word(&displayBand[bandSelectorValue]));    
                    strcpy_P(tuneStepDisplay, (PGM_P)pgm_read_word(&tuneStepsDisplay[tuneStepValue]));  
                    
                    display.setTextSize(1);
                    display.setCursor(54, 33);
                    display.print(tuneStepDisplay);
                    
                    display.setCursor(92, 57);
                    display.print(interFrequency);
                    display.print("k");
                    
                    display.setCursor(110, 23);
                    display.print(setFrequencyDisplay(frequency));

                    display.setCursor(110, 33);
                    display.print(setInterFrequentieDisplay(interFrequency));

                    display.setCursor(91, 28);
                    display.print(setRxTxDisplay(rxtxSwitch));
                    
                    display.setTextSize(2);
                    display.setCursor(0, 25);
                    display.print(bandSelectorTypeDisplay);

                    setBargraph(tunePointer,signalMeterRemap);
                
                    display.display();
                }    
            }
            

        private:

            /***************************************************************************************/
            /*! @brief  setup the static Screen elements                                          */
            /***************************************************************************************/
            void setScreenStatic() {
                display.setTextColor(WHITE);    
                display.setTextSize(1);

                display.drawLine(0, 20, 127, 20, WHITE);
                display.drawLine(0, 43, 127, 43, WHITE);
                display.drawLine(105, 24, 105, 39, WHITE);
                display.drawLine(87, 24, 87, 39, WHITE);
                display.drawLine(87, 48, 87, 63, WHITE);
                display.drawLine(15, 55, 82, 55, WHITE);
                    
                display.setCursor(59, 23);
                display.print("STEP");
                    
                display.setCursor(92, 48);
                display.print("IF:");

                display.setCursor(0, 48);
                display.print("TU");

                display.setCursor(0, 57);
                display.print("SM");
    
            }  

            /***************************************************************************************/
            /*! @brief  Set the type to kHz or MHz for the display    
                @param  freq The currenct frequency 
                @return kHz or MHz 
            */
            /***************************************************************************************/
            char* setFrequencyDisplay(int freq) {
                char* returnDisplay =(char*)"  ";
                if (freq < 1000000) { 
                returnDisplay = (char*)"kHz";
                } else {
                returnDisplay = (char*)"MHz";
                }
                return returnDisplay;
            }

            /***************************************************************************************/
            /*! @brief  Set the interfrequentye for the display     
                @param  freq The currenct frequency 
                @return VFO or L O 
            */
            /***************************************************************************************/
            char* setInterFrequentieDisplay(int freq) {
                char* returnDisplay =(char*)"  ";
                if (freq == 0) { 
                    returnDisplay =(char*)"VFO";
                } else {
                    returnDisplay=(char*)"L O";
                }
                return returnDisplay;
            }

            /***************************************************************************************/
            /*! @brief  Set on display TX OR RX    
                @param  rxtxSwitch true or fals 
                @return TX or RX 
            */
            /***************************************************************************************/
            char* setRxTxDisplay(bool rxtxSwitch) {
                char* returnDisplay =(char*)"   "; 
                if (rxtxSwitch == true){
                    returnDisplay = (char*)"TX";
                } else { 
                    returnDisplay = (char*)"RX";
                }
                return returnDisplay;
            } 

            /***************************************************************************************/
            /*! @brief  Set on display the recalculated tunepoint    
                @param  tunePointer      The tune pointer 
                @param singalMeterRemap  The recalculated tune pointer
            */
            /***************************************************************************************/
            void setBargraph(byte tunePointer, byte signalMeterRemap) {
                byte y = map(tunePointer, 1, 42, 1, 14);
                display.setTextSize(1);
                display.fillRect(10 + (5 * y), 48, 2, 6, WHITE);
                
                for (int x = signalMeterRemap; x > 0; x--) {
                    display.fillRect(10 + (5 * x), 58, 2, 6, WHITE);
                }
            }       

            /***************************************************************************************/
            /*! @brief  Set the format of the frequency value on display     
                @param  frequency        The current frequency 
            */
            /***************************************************************************************/
            void setFormattedFrequency(unsigned long frequency) {
                unsigned int m = frequency / 1000000;
                unsigned int k = (frequency % 1000000) / 1000;
                unsigned int h = (frequency % 1000) / 1;
                
                //display.clearDisplay();
                display.setTextSize(2);
                char buffer[15] = "";
                
                if (m < 1) {
                    display.setCursor(41, 1);
                    sprintf(buffer, "%003d.%003d", k, h);
                } else if (m < 100) {
                    display.setCursor(5, 1);
                    sprintf(buffer, "%2d.%003d.%003d", m, k, h);
                } else if (m >= 100) {
                    unsigned int h = (frequency % 1000) / 10;
                    display.setCursor(5, 1);
                    sprintf(buffer, "%2d.%003d.%02d", m, k, h);
                }
                display.print(buffer);
            }
    };

    Display displayManager;

}