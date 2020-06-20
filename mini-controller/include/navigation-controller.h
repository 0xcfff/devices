#ifndef _NAVIGATION_CONTROLLER_H_
#define _NAVIGATION_CONTROLLER_H_

#include <stdint.h>

#include <map>
#include <utility>

#include "mode-controller.h"
#include "navigation-model.h"
#include "navigation-view.h"


class MainController{

    public:
        MainController(uint8_t modePin, uint8_t confirmPin, uint8_t cancelPin, NavigationView * view, std::vector<std::pair<NavigationTargetDescriptor *, ModeController *>> * childControllers = nullptr);
        ~MainController();

        bool handle();

        void addChildModeController(NavigationTargetDescriptor * controllerInfo, ModeController * controller);

    private:
        uint8_t _modePin;
        uint8_t _confirmPin;
        uint8_t _cancelPin;
        std::map<uint8_t, ModeController *> _modeControllers;
        NavigationView * _view;
        NavigationModel _model;
};


#endif // _NAVIGATION_CONTROLLER_H_