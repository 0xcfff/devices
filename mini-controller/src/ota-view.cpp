#include <macro-flags.h>

#include "ota-view.h"

OtaView::OtaView(Display * display) :
  _display(display)
{

}

void OtaView::drawModel(OtaModel * model) {
    auto otaState = model->otaStatus;

    char ipAddress[20];
    sprintf(ipAddress, "%i.%i.%i.%i", 
        (int)((model->ownIP >> 24) & 0xFF),
        (int)((model->ownIP >> 16) & 0xFF),
        (int)((model->ownIP >> 8) & 0xFF),
        (int)((model->ownIP >> 0) & 0xFF));

    switch (otaState)
    {
        case OTAUPDATERSTATUS_WAITINGCONNECTION: {
            _display->draw2CTableViewLine(1, 48, "Status:", "Waiting...");
            _display->draw2CTableViewLine(2, 48, "WIFI AP:", model->apName);
            _display->draw2CTableViewLine(3, 48, "OTA IP:", ipAddress);
            break;
        }
        case OTAUPDATERSTATUS_INPROGRESS: {
            break;
        }
        case OTAUPDATERSTATUS_COMPLETED: {
            break;
        }
        case OTAUPDATERSTATUS_DISABLED: {
            break;
        }
        case OTAUPDATERSTATUS_ERROR: {
            break;
        }
        default: {
            _display->drawTextLine(0, "OTA Status Unknown");
            break;
        }
    }

    // if (otaState == OTAUPDATERSTATUS_NONE) {
        
    // } else {
    //     _display->drawTextLine(0, "OTA Info");
    //     const char * status = "Unknown";
    //     if (model->otaStatus == OTAMODELSTATE_UPDATECOMPLETED) {
    //         status = "Completed";
    //     } else if (model->otaStatus == OTAMODELSTATE_ERROR) {
    //         status = "Error";
    //     } else if (model->otaStatus == OTAMODELSTATE_INPROGRESS) {
    //         status = "In Progress...";
    //     } else if (model->otaStatus == OTAMODELSTATE_WAITING) {
    //         status = "Waiting...";
    //     } else if (model->otaStatus == OTAMODELSTATE_DISABLED) {
    //         status = "Disabled";
    //     }

    // }
}
