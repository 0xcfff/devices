#ifndef _NAVIGATION_CONTROLLER_H_
#define _NAVIGATION_CONTROLLER_H_

#include <stdint.h>

#include <map>
#include <utility>

#include "button.h"
#include "mode-controller.h"
#include "navigation-model.h"
#include "navigation-view.h"

#define NAVCONTROLLERSTATE_EMPTY                 0x00
#define NAVCONTROLLERSTATE_INITIALDRAWDONE       0x01

enum NavigationControllerState : uint8_t {
    NAVCONTROLLERSTATE_NAVIGATINGTHROUGHCONTROLLERS    = 1,
    NAVCONTROLLERSTATE_ENTEREDNESTEDCONTROLLER         = 2
};


class MainController{

    private:
        enum HandleInputResult{
            HANDLEINPUTRESULT_NONE                          = 0,
            HANDLEINPUTRESULT_HANDLEDBYSELF                 = 1,
            HANDLEINPUTRESULT_HANDLEDBYNESTEDCONTROLLER     = 2,
            HANDLEINPUTRESULT_ENTEREDNESTEDCONTROLLER       = 3,
            HANDLEINPUTRESULT_LEFTNESTEDCONTROLLER          = 4
        };

    public:
        MainController(Button * modeButton, Button * confirmButton, Button * cancelButton, NavigationView * view, std::vector<std::pair<NavigationTargetDescriptor *, ModeController *>> * childControllers = nullptr);
        ~MainController();

        bool handle();

        void addChildModeController(NavigationTargetDescriptor * controllerInfo, ModeController * controller);
        bool isSleeping();

    private:
        bool redrawView();
        
        bool processSleepMode();
        HandleInputResult processUserInput();
        HandleInputResult handleUserInput(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state);
        HandleInputResult handleUserInputWhenNavigate(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state);
        HandleInputResult handleUserInputWhenEnteredNestedController(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state);

        static ModeControllerCommandButtonAction translateButtonActionToMCButtonAction(ButtonAction action);

    private:
        Button * _modeButton;
        Button * _confirmButton;
        Button * _cancelButton;
        std::map<uint8_t, ModeController *> _modeControllers;
        NavigationView * _view;
        NavigationModel _model;
        uint8_t _stateFlags;
        NavigationControllerState _state;
        ModeController * _currentController;
        unsigned long _lastUserActionMillis;
        bool _sleepMode;
};


#endif // _NAVIGATION_CONTROLLER_H_