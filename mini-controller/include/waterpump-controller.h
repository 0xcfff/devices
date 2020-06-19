#ifndef _WATERPUMP_CONTROLLER_H_
#define _WATERPUMP_CONTROLLER_H_

#include <stdint.h>

#include <RF24.h>

#include "mode-controller.h"

#include "waterpump-model.h"
#include "waterpump-view.h"

class WaterPumpController : public ModeController{
    public:
        WaterPumpController(WaterPumpView * view, RF24 * radio);

        bool activate() override;
        bool deactivate() override;

        bool handleTick() override;
        ModeControllerHandleUserInputResult handleUserInput(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state) override;

    private:
        WaterPumpConnectionState testConnection();

    private:
        WaterPumpView * _view;
        RF24 * _radio;
        WaterPumpModel _model;
};


#endif  // _WATERPUMP_CONTROLLER_H_