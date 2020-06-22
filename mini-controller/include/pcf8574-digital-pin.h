#ifndef _PCF8574_DIGITAL_PIN_H_
#define _PCF8574_DIGITAL_PIN_H_

#include <utility>
#include <stdint.h>

#include <pcf8574_esp.h>

#include "pins.h"

// TODO: add inverse feature
#define PCF8574DIGPINCFG_DEFAULT_HIGH                   0x01
#define PCF8574DIGPINCFG_DEFAULT_LOW                    0x02
#define PCF8574DIGPINCFG_SOFTWAREINTERRUPTS_ENABLE      0x04

#define PCF8574DIGPINSTATE_EMPTY                        0x00
#define PCF8574DIGPINSTATE_ACTIVE                       0x01

class PCF8574DigitalPin : public DigitalPin{
    public:
        PCF8574DigitalPin(PCF857x * pcf8574, uint8_t pinNumber, uint8_t configFlags);

        uint8_t supportedFeatures() override;
        bool digitalRead() override;
        bool digitalWrite(bool isHigh) override;

        bool attachInterruptHandler(std::function<void(DigitalPin *)> handler, uint8_t mode) override;
        bool detachInterruptHandler() override;
        bool riseInterrupt();

        bool begin() override;
        bool end() override;

    private:
        

    private:
        PCF857x * _pcf8574;
        uint8_t _pinNumber;
        uint8_t _configFlags;
        uint8_t _stateFlags;
        std::function<void(DigitalPin *)> _interruptHandler;

};

#endif  // _PCF8574_DIGITAL_PIN_H_