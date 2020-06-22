#ifndef _ARDUINO_DIGITAL_PIN_H_
#define _ARDUINO_DIGITAL_PIN_H_

#include <utility>

#include <stdint.h>

#include "pins.h"

// TODO: add inverse feature
#define ARDUINODIGPINCFG_EMPTY                          0x00
#define ARDUINODIGPINCFG_DEFAULT_HIGH                   0x01
#define ARDUINODIGPINCFG_DEFAULT_LOW                    0x02
#define ARDUINODIGPINCFG_HARDWAREINTERRUPTS_ENABLE      0x04

#define ARDUINODIGPINSTATE_EMPTY                        0x00
#define ARDUINODIGPINSTATE_ACTIVE                       0x01
#define ARDUINODIGPINSTATE_SUPPORTSDIRECTREAD           0x02
#define ARDUINODIGPINSTATE_SUPPORTSDIRECTWRITE          0x04
#define ARDUINODIGPINSTATE_ISSET                        0x08

#define ARDUINODIGPINMODE_UNDEFINED                     0xFF


class ArduinoDigitalPin : public DigitalPin{
    public:
        ArduinoDigitalPin(uint8_t pinNumber, uint8_t pinMode = ARDUINODIGPINMODE_UNDEFINED, uint8_t configFlags = ARDUINODIGPINCFG_EMPTY);

        uint8_t supportedFeatures() override;
        bool digitalRead() override;
        bool digitalWrite(bool isHigh) override;

        bool attachInterruptHandler(std::function<void(DigitalPin *)> handler, uint8_t mode) override;
        bool detachInterruptHandler() override;

        bool begin() override;
        bool end() override;

    private:
        

    private:
        uint8_t _pinNumber;
        uint8_t _pinMode;
        uint8_t _configFlags;
        uint8_t _stateFlags;
    
};

#endif  // _ARDUINO_DIGITAL_PIN_H_