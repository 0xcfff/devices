#ifndef _IDLE_CONTROLLER_H_
#define _IDLE_CONTROLLER_H_

#include "display.h"
#include "mode-controller.h"

#define IDLECONTROLLERSTATE_EMPTY           0x00
#define IDLECONTROLLERSTATE_SLEEPMODE       0x01

class IdleController : public ModeController{
    public:
        IdleController(Display * view, uint16_t splashDisplayDurationMillis);

        bool activate() override;
        bool deactivate() override;

        bool handleTick() override;
        ModeControllerHandleUserInputResultData handleUserInput(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state) override;

    private:
        bool activateSleepMode();
        bool deactivateSleepMode();

    private:
        Display * _view;
        uint16_t _splashDisplayDurationMillis;
        uint8_t _stateFlags;
        unsigned long _activatedAtMillis;
};

#endif  // _IDLE_CONTROLLER_H_