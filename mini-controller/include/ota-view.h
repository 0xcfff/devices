#ifndef _OTA_VIEW_H_
#define _OTA_VIEW_H_

#include "display.h"

#include "ota-model.h"

class OtaView {
    public:
        OtaView(Display * display);

        void drawModel(OtaModel * model);

    private:
        Display * _display;
};

#endif  // _OTA_VIEW_H_