#ifndef _IDLE_CONTROLLER_H_
#define _IDLE_CONTROLLER_H_

#include "display.h"
#include "mode-controller.h"

class IdleController : public ModeController{
    private:
        enum IdleControllerState{
            IDLECONTROLLERSTATE_INACTIVE            = 0,
            IDLECONTROLLERSTATE_SHOWSLEEPSPLASH     = 1,
            IDLECONTROLLERSTATE_SLEEPING            = 2,
            IDLECONTROLLERSTATE_SHOWWAKEUPSPLASH    = 3
        };

    public:
        IdleController(Display * view, uint16_t splashDisplayDurationMillis);

        bool activate() override;
        bool deactivate() override;

        bool handleTick() override;
        ModeControllerHandleUserInputResultData handleUserInput(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state) override;
        ModeControllerSystemEventResult handleSystemEvent(ModeControllerSystemEvent systemEvent);

    private:
        bool activateSleepMode();
        bool deactivateSleepMode();

        bool redrawView();

        ModeControllerHandleUserInputResultData handleUserInputOnSplash(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state);
        ModeControllerHandleUserInputResultData handleUserInputOnSleep(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state);

    private:
        Display * _view;
        uint16_t _splashDisplayDurationMillis;
        IdleControllerState _state;
        unsigned long _stateActivatedAtMillis;
};

#endif  // _IDLE_CONTROLLER_H_