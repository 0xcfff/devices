#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <stdint.h>

#include <pcf8574_esp.h>

#define KBD_STATE_NONE      0
#define KBD_STATE_INIT      1


class Keyboard {
    public:
        Keyboard(PCF857x * pcf8574, uint8_t menuButtonPcfPin, uint8_t okButtonPcfPin, uint8_t cancelButtonPcfPin, uint8_t irqControllerPin);
        ~Keyboard();

        bool begin();
        bool end();

    private:
        void attachInterruptHandler();
        void detachInterruptHandler();
        
        ICACHE_RAM_ATTR void onInterrupt();
        
    private:
        PCF857x * _pcf8574;
        uint8_t _menuButtonPcfPin; 
        uint8_t _okButtonPcfPin; 
        uint8_t _cancelButtonPcfPin; 
        uint8_t _irqControllerPin;
        uint8_t _stateFlags;

};

#endif  // _KEYBOARD_H_