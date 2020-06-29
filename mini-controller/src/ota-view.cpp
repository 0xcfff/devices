#include <macro-flags.h>

#include "ota-view.h"

OtaView::OtaView(Display * display) :
  _display(display)
{

}

void OtaView::drawModel(OtaModel * model) {
    auto otaState = model->otaState;

    if (otaState == OTAMODELSTATE_NONE) {
        auto rawDisplay = _display->getRawDisplay();
        rawDisplay->setFont(u8g2_font_6x12_t_cyrillic);        
        rawDisplay->enableUTF8Print();

        rawDisplay->setCursor(10,16);
        rawDisplay->print("No Status");
    } else {
        _display->drawTextLine(0, "OTA Info");
        const char * status = "Unknown";
        if (model->otaState == OTAMODELSTATE_UPDATECOMPLETED) {
            status = "Completed";
        } else if (model->otaState == OTAMODELSTATE_ERROR) {
            status = "Error";
        } else if (model->otaState == OTAMODELSTATE_INPROGRESS) {
            status = "In Progress...";
        } else if (model->otaState == OTAMODELSTATE_WAITING) {
            status = "Waiting...";
        } else if (model->otaState == OTAMODELSTATE_DISABLED) {
            status = "Disabled";
        }

        _display->draw2CTableViewLine(1, 48, "Status:", status);
        _display->draw2CTableViewLine(2, 48, "WIFI AP:", model->apName);

        char buff[20];
        sprintf(buff, "%i.%i.%i.%i", 
            (int)((model->ownIP >> 24) & 0xFF),
            (int)((model->ownIP >> 16) & 0xFF),
            (int)((model->ownIP >> 8) & 0xFF),
            (int)((model->ownIP >> 0) & 0xFF));

        _display->draw2CTableViewLine(3, 48, "OTA IP:", buff);
    }
}
