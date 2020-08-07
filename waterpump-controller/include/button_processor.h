#ifndef _BUTTON_PROCESSOR_H_
#define _BUTTON_PROCESSOR_H_

#include <JC_Button.h>

#include "led.h"
#include "ota-updater.h"
#include "wifi-ap.h"


#define CTLBTNPROCSTATE_EMPTY             0x00
#define CTLBTNPROCSTATE_ACTIVE            0x01
#define CTLBTNPROCSTATE_OTAENABLED     0x02


class ControlButtonProcessor{
    public:
        ControlButtonProcessor(Button & controlButton, OtaUpdater & otaUpdater, WiFiAP & wifiAp, Led & stateIndicatorLed, uint16_t wifiAutoDisableSec, uint16_t buttonLongPressMsec, uint16_t ledOtaBlinkIntervalMsec);

        bool begin();
        bool end();

        bool handle();

        bool enableOtaMode(bool enable);

    private:

        void onControlButtonClick();
        void onControlButtonLongPress();

    private:
        uint16_t _stateFlags;
        unsigned long _lastLedFlipAt;

        uint16_t _buttonLongPressMsec;
        uint16_t _wifiAutoDisableSec;
        uint16_t _ledOtaBlinkIntervalMsec;
        Button & _controlButton;
        OtaUpdater & _otaUpdater; 
        WiFiAP & _wifiAp;
        Led & _stateIndicatorLed;
};


#endif