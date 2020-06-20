#ifndef _MAIN_CONTROLLER_H_
#define _MAIN_CONTROLLER_H_

#include <stdint.h>
#include <vector>

#include "mode-controller.h"
#include "mode-selector-presenter.h"

enum ButttonsProcessingResult : uint8_t {
    PROCESSOR_HANDLE_RESULT_SUCCESS     = 1,
    PROCESSOR_HANDLE_RESULT_ERROR       = 0,
    PROCESSOR_HANDLE_RESULT_IDLE        = 2
};

#define ROOTCTL_NO_MODECTL_SELECTED     0xFF


class MainController{
    private:
        struct ModeControllerNode {
            ModeDescription * controllerInfo;
            ModeController * controllerInstance;
        };

    public:
        MainController(uint8_t modePin, uint8_t confirmPin, uint8_t cancelPin, ModeSelectorPresenter * modeSelectionPresenter);
        ~MainController();

        ButttonsProcessingResult handle();

        void addChildModeController(ModeDescription * controllerInfo, ModeController * controller);

    private:
        uint8_t _modePin;
        uint8_t _confirmPin;
        uint8_t _cancelPin;
        uint8_t _defaultControllerIndex;
        uint8_t _currentControllerIndex;
        std::vector<ModeControllerNode*> _modeControllers;
        ModeSelectorPresenter * _modeSelectionPresenter;
};


#endif // _MAIN_CONTROLLER_H_