#include <macro-flags.h>

#include "ota-view.h"

OtaView::OtaView(Display * display) :
  _display(display)
{

}

void OtaView::drawModel(OtaModel * model) {
    auto otaState = model->otaState;
    auto rawDisplay = _display->getRawDisplay();

    if (otaState == OTAMODELSTATE_NONE) {
        rawDisplay->setFont(u8g2_font_6x12_t_cyrillic);        
        rawDisplay->enableUTF8Print();

        rawDisplay->setCursor(10,16);
        rawDisplay->print("No Status");
    } else {
        rawDisplay->setFont(u8g2_font_6x12_t_cyrillic);        
        rawDisplay->enableUTF8Print();

        rawDisplay->setCursor(10,16);
        char buff[20];
        sprintf(buff, "%i.%i.%i.%i", 
            (int)((model->ownIP >> 24) & 0xFF),
            (int)((model->ownIP >> 16) & 0xFF),
            (int)((model->ownIP >> 8) & 0xFF),
            (int)((model->ownIP >> 0) & 0xFF));

        rawDisplay->print(buff);
    }
}
