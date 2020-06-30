#ifndef _WATERPUMP_CONTROLLER_H_
#define _WATERPUMP_CONTROLLER_H_

#include <stdint.h>

#include <RF24.h>

#include "mode-controller.h"

#include "waterpump-model.h"
#include "waterpump-view.h"

class WaterPumpController : public ModeController{
    private:
        enum WaterPumpControllerState : uint8_t {
            WATERPUMPCTLSTATE_INACTIVE              = 0,
            WATERPUMPCTLSTATE_MONITORCONNECTION     = 1,
        };

    public:
        WaterPumpController(WaterPumpView * view, RF24 * radio, uint16_t pingIntervalMsec);

        bool activate() override;
        bool deactivate() override;

        bool handleTick() override;
        ModeControllerHandleUserInputResultData handleUserInput(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state) override;

    private:
        bool refreshConnectionState(bool withPing);
        bool redrawView();

        void onReceiveRFMessage();

    private:
        WaterPumpView * _view;
        RF24 * _radio;
        WaterPumpControllerState _state;
        uint16_t _pingIntervalMsec;
        unsigned long _lastPingTimeMillis;
        WaterPumpModel _model;
};


#endif  // _WATERPUMP_CONTROLLER_H_