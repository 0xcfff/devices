#ifndef _WATERPUMP_VIEW_H_
#define _WATERPUMP_VIEW_H_

#include "main-view.h"
#include "waterpump-model.h"

class WaterPumpView{
    public:
        WaterPumpView(MainView * view);

        void drawModel(WaterPumpModel model);

    private:
        MainView * _view;

};

#endif  // _WATERPUMP_VIEW_H_