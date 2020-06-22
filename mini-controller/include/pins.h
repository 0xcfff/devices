#ifndef _PINS_H_
#define _PINS_H_

#include <utility>

#include <stdint.h>

#define PIN_FEATURE_DIGITALREAD                 0x01
#define PIN_FEATURE_DIGITALWRITE                0x02
#define PIN_FEATURE_ATTACHINTERRUPTHANDLER      0x04

class DigitalPin {
    public:
        virtual uint8_t supportedFeatures() = 0;
        virtual bool digitalRead() = 0;
        virtual bool digitalWrite(bool isHigh) = 0;

        virtual bool attachInterruptHandler(std::function<void(DigitalPin *)> handler, uint8_t mode) = 0;
        virtual bool detachInterruptHandler() = 0;

        virtual bool begin() = 0;
        virtual bool end() = 0;

};

#endif // _PINS_H_