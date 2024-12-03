#include <Rotary.h>                        // Arduino library for reading rotary directions that output a 2-bit gray code. Ben  Buxton https://github.com/brianlow/Rotary

namespace MyRotaryHandler {
    


    Rotary r = Rotary(2, 3);               // Initialize Rotary opbject r (Type Rotary) a. assign pin 2 and pin 3 b. set Initial state c. set default inverter
    class RotaryHandler {

        private:

            static constexpr uint32_t MIN_FREQUENCY = 10000;
            static constexpr uint32_t MAX_FREQUENCY = 225000000;
            static constexpr uint8_t  MIN_TUNE_POINTER = 1;
            static constexpr uint8_t  MAX_TUNE_POINTER = 42;

            uint32_t frequency;
            uint32_t frequencyStep;
            uint8_t  tunePointer;

            
            
        public:

            uint32_t getFrequency() const { return frequency; }
            uint8_t  getTunePointer() const { return tunePointer; }
            void     setFrequency(uint32_t  freq ) { frequency = freq; }
            void     setTunePointer(uint8_t tp) { tunePointer= tp; }
            void     setFrequencyStep(uint32_t fs) {frequencyStep =fs;}

            void handleRotary() {
                char result = r.process();
                setValues(result);
            }

            void init(){
                pinMode(2, INPUT_PULLUP);
                pinMode(3, INPUT_PULLUP);
                PCICR |= (1 << PCIE2);
                PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);
                sei();
            }
  
        private:

            void setValues(char direction) {
                if (direction == DIR_CW) {
                    frequency = min(frequency + frequencyStep, MAX_FREQUENCY);
                    tunePointer = (tunePointer % MAX_TUNE_POINTER) + 1;
                } else if (direction == DIR_CCW) {
                    frequency = max(frequency - frequencyStep, MIN_FREQUENCY);
                    tunePointer = (tunePointer > MIN_TUNE_POINTER) ? tunePointer - 1 : MAX_TUNE_POINTER;
                }

                //Frequency can not by smaller than the frequencyStep 
                if (frequencyStep == 1000000 && frequency <= 1000000)  {              
                    frequency= 1000000; 
                } 
            }
    
     };

    // Global instance of RotaryHandler
     RotaryHandler interruptManager;

    // Interrupt Service Routine
    ISR(PCINT2_vect) {
        interruptManager.handleRotary();
    }
}