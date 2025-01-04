

namespace MyDisplay {

            /***************************************************************************************/
            /*! @brief Initialize Adafruit_SSD1306 object type Adafruit_SSD1306
                        width 128 pixels hight 64 pixels, 
                        using the Wire object(library) by reference                                */
            /***************************************************************************************/
             Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);
       
            /***************************************************************************************/
            /*! @brief  Initial Screen object                                                      */
            /***************************************************************************************/
            void Screen::init() {

                timeNow = 0;          // The current time
                period = 100;         // Used for calculation the performance time .
                
                Wire.begin();
                display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
                display.clearDisplay();
                display.setTextColor(WHITE);
                display.display();
            }

            /***************************************************************************************/
            /*! @brief  Initial timesetup                                                          */
            /***************************************************************************************/
            void Screen::setTimeNow() {
                timeNow = millis();
            }
            
            /***************************************************************************************/
            /*! @brief  Start text to screen to display                                            */
            /***************************************************************************************/
            void Screen::setStartupText() {
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
            void Screen::setScreen(
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
                    display.print(setFrequencyScreen(frequency));

                    display.setCursor(110, 33);
                    display.print(setInterFrequentieScreen(interFrequency));

                    display.setCursor(91, 28);
                    display.print(setRxTxScreen(rxtxSwitch));
                    
                    display.setTextSize(2);
                    display.setCursor(0, 25);
                    display.print(bandSelectorTypeDisplay);

                    setBargraph(tunePointer,signalMeterRemap);
                
                    display.display();
                }    
            }
            

            /***************************************************************************************/
            /*! @brief  setup the static Screen elements                                          */
            /***************************************************************************************/
            void Screen::setScreenStatic() {
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
            char* Screen::setFrequencyScreen(long freq) {
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
            char* Screen::setInterFrequentieScreen(long freq) {
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
            char* Screen::setRxTxScreen(bool rxtxSwitch) {
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
            void Screen::setBargraph(byte tunePointer, byte signalMeterRemap) {
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
            void Screen::setFormattedFrequency(unsigned long frequency) {
                unsigned int m = frequency / 1000000;
                unsigned int k = (frequency % 1000000) / 1000;
                unsigned int h = (frequency % 1000) / 1;
                
                display.setTextSize(2);
                char buffer[15] = "";
                
                if (m < 1) {
                    display.setCursor(41, 1);
                    sprintf(buffer, "%03d.%03d", k, h);
                } else if (m < 100) {
                    display.setCursor(5, 1);
                    sprintf(buffer, "%2d.%03d.%03d", m, k, h);
                } else if (m >= 100) {
                    unsigned int h = (frequency % 1000) / 10;
                    display.setCursor(5, 1);
                    sprintf(buffer, "%2d.%03d.%02d", m, k, h);
                }
                display.print(buffer);
            }
     
    Screen displayManager ;
}    

