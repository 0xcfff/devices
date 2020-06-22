#include <utility>
#include <FunctionalInterrupt.h>
#include <tnlog.h>

#include "shared_macroses.h"

#include "keyboard.h"

Keyboard::Keyboard(PCF857x * pcf8574, uint8_t menuButtonPcfPin, uint8_t okButtonPcfPin, uint8_t cancelButtonPcfPin, uint8_t irqControllerPin) :
    _pcf8574(pcf8574),
    _menuButtonPcfPin(menuButtonPcfPin),
    _okButtonPcfPin(okButtonPcfPin),
    _cancelButtonPcfPin(cancelButtonPcfPin),
    _irqControllerPin(irqControllerPin),
    _stateFlags(0)
{

}

Keyboard::~Keyboard(){
    end();
}

bool Keyboard::begin(){
    bool result = false;
    if (!IS_FLAG_SET(KBD_STATE_INIT, _stateFlags)) {
        attachInterruptHandler();
        SET_FLAG(KBD_STATE_INIT, _stateFlags);
        result = true;
    }
    return result;
}

bool Keyboard::end(){
    bool result = false;
    if (IS_FLAG_SET(KBD_STATE_INIT, _stateFlags)) {
        detachInterruptHandler();
        RESET_FLAG(KBD_STATE_INIT, _stateFlags);
        result = true;
    }
    return result;
}


void Keyboard::attachInterruptHandler(){
    std::function<void(void)> handler = std::bind(&Keyboard::onInterrupt, this);
    attachInterrupt(digitalPinToInterrupt(_irqControllerPin), handler, CHANGE);
}

void Keyboard::detachInterruptHandler(){
    detachInterrupt(digitalPinToInterrupt(_irqControllerPin));
}

void Keyboard::onInterrupt(){
    LOG_DEBUGLN("Button pressed!");
}
