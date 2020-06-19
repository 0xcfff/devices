#ifndef _WATERPUMP_VIEW_H_
#define _WATERPUMP_VIEW_H_

#include "display.h"
#include "waterpump-model.h"

class WaterPumpView{
    public:
        WaterPumpView(Display * view);

        void drawModel(WaterPumpModel model);

    private:
        Display * _view;

};

#endif  // _WATERPUMP_VIEW_H_