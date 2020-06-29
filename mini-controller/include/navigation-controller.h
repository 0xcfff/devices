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
#define NAVCONTROLLERSTATE_MODEENTERED           0x02


class MainController{

    public:
        MainController(Button * modeButton, Button * confirmButton, Button * cancelButton, NavigationView * view, std::vector<std::pair<NavigationTargetDescriptor *, ModeController *>> * childControllers = nullptr);
        ~MainController();

        bool handle();

        void addChildModeController(NavigationTargetDescriptor * controllerInfo, ModeController * controller);

    private:
        bool redrawView();

    private:
        Button * _modeButton;
        Button * _confirmButton;
        Button * _cancelButton;
        std::map<uint8_t, ModeController *> _modeControllers;
        NavigationView * _view;
        NavigationModel _model;
        uint8_t _stateFlags;
        ModeController * _currentController;
};


#endif // _NAVIGATION_CONTROLLER_H_