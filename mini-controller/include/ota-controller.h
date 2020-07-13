#ifndef _OTA_CONTROLLER_H_
#define _OTA_CONTROLLER_H_

#include <ota-updater.h>
#include <wifi-ap.h>

#include "mode-controller.h"

#include "ota-model.h"
#include "ota-view.h"

class OtaController : public ModeController {
    public:
        OtaController(OtaUpdater * otaUpdater, WiFiAP * wifiAp, uint16_t waitIntervalMsec, OtaView * view);

        bool activate() override;
        bool deactivate() override;

        bool handleTick() override;
        ModeControllerHandleUserInputResultData handleUserInput(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state) override;
        ModeControllerSystemEventResult handleSystemEvent(ModeControllerSystemEvent systemEvent);

    private:
        bool activateOta();
        bool deactivateOta();

    private:
        OtaUpdater * _otaUpdater;
        WiFiAP * _wifiAp;
        OtaView * _view;
        uint16_t _waitIntervalMsec;
        bool _isActive;
        OtaModel _model;
};

#endif  // _OTA_CONTROLLER_H_