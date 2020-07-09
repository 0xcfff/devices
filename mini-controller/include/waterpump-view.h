#ifndef _WATERPUMP_VIEW_H_
#define _WATERPUMP_VIEW_H_

#include "display.h"
#include "waterpump-model.h"

class WaterPumpView{
    public:
        WaterPumpView(Display * display);

        void drawModel(WaterPumpModel *model);

    private:
        static const char * textualizeConnectionState(WaterPumpConnectionState connectionState);
        static const char * textualizeWaterPumpAction(WaterPumpAction action);

    private:
        Display * _display;

};

#endif  // _WATERPUMP_VIEW_H_