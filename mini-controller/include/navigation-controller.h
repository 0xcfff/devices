#ifndef _NAVIGATION_CONTROLLER_H_
#define _NAVIGATION_CONTROLLER_H_

#include <stdint.h>

#include <vector>
#include <utility>

#include "mode-controller.h"
#include "navigation-model.h"
#include "navigation-view.h"

#define ROOTCTL_NO_MODECTL_SELECTED     0xFF


class MainController{

    public:
        MainController(uint8_t modePin, uint8_t confirmPin, uint8_t cancelPin, NavigationView * view, std::vector<std::pair<ModeDescription *, ModeController *>> * childControllers = nullptr);
        ~MainController();

        bool handle();

        void addChildModeController(ModeDescription * controllerInfo, ModeController * controller);

    private:
        uint8_t _modePin;
        uint8_t _confirmPin;
        uint8_t _cancelPin;
        uint8_t _defaultControllerIndex;
        uint8_t _currentControllerIndex;
        std::vector<std::pair<ModeDescription *, ModeController *>> _modeControllers;
        NavigationView * _view;
};


#endif // _NAVIGATION_CONTROLLER_H_