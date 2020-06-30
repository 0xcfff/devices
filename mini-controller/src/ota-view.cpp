#include <macro-flags.h>

#include "ota-view.h"

OtaView::OtaView(Display * display) :
  _display(display)
{

}

void OtaView::drawModel(OtaModel * model) {
    auto otaState = model->otaStatus;

    switch (otaState)
    {
        case OTAUPDATERSTATUS_WAITINGCONNECTION: {
            char ipAddress[20];
            sprintf(ipAddress, "%i.%i.%i.%i", 
                (int)((model->ownIP >> 24) & 0xFF),
                (int)((model->ownIP >> 16) & 0xFF),
                (int)((model->ownIP >> 8) & 0xFF),
                (int)((model->ownIP >> 0) & 0xFF));

            _display->drawTextLine(0, "OTA Firmware Update");
            _display->draw2CTableViewLine(1, 48, "Status:", "Waiting...");
            _display->draw2CTableViewLine(2, 48, "WIFI AP:", model->apName);
            _display->draw2CTableViewLine(3, 48, "OTA IP:", ipAddress);
            break;
        }
        case OTAUPDATERSTATUS_INPROGRESS: {
            char progress [10];
            sprintf(progress, "%i%%", (int)model->otaProgress);
            _display->drawTextLine(0, "OTA Firmware Update");
            _display->draw2CTableViewLine(1, 48, "Status:", "In Progress...");
            _display->draw2CTableViewLine(2, 48, "Progress:", progress);
            break;
        }
        case OTAUPDATERSTATUS_COMPLETED: {
            char restartIn [10];
            sprintf(restartIn, "%i sec", (int)model->otaProgress);
            _display->drawTextLine(0, "OTA Firmware Update");
            _display->draw2CTableViewLine(1, 48, "Status:", "Completed");
            _display->draw2CTableViewLine(2, 48, "Restart in:", restartIn);
            break;
        }
        case OTAUPDATERSTATUS_DISABLED: {
            _display->drawTextLine(0, "OTA Firmware Update");
            _display->draw2CTableViewLine(1, 48, "Status:", "Disabled");
            _display->drawTextLine(3, "Click OK to enable");
            break;
        }
        case OTAUPDATERSTATUS_ERROR: {
            _display->drawTextLine(0, "OTA Firmware Update");
            _display->draw2CTableViewLine(1, 48, "Status:", "Error");
            _display->drawTextLine(3, "Click OK to retry");
            break;
        }
        default: {
            _display->drawTextLine(0, "OTA Status Unknown");
            break;
        }
    }
}
