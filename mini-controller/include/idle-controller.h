#ifndef _IDLE_CONTROLLER_H_
#define _IDLE_CONTROLLER_H_

#include "main-view.h"
#include "mode-controller.h"

class IdleController : public ModeController{
    public:
        IdleController(MainView * view);

        ModeControllerHandleUserInputResult handleUserInput(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state) override;

    private:
        MainView * _view;
};

#endif  // _IDLE_CONTROLLER_H_