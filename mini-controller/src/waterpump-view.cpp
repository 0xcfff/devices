#include "waterpump-view.h"

WaterPumpView::WaterPumpView(Display * view):
    _display(view)
{
}

void WaterPumpView::drawModel(WaterPumpModel * model){
    _display->drawTextLine(0, "Water Pump");
    _display->draw2CTableViewLine(1, 48, "Line:", textualizeConnectionState(model->connectionState));
    if (model->connectionState == WPCONNECTION_CONNECTED || model->connectionState == WPCONNECTION_CONNECTEDBADSYSIGNAL)
    {
        _display->draw2CTableViewLine(2, 48, "State:", model->pumpIsWorking ? "Working..." : "Stopped");
        if (model->pumpIsWorking) 
        {
            char buff[16];
            snprintf(buff, 16, "%u", (unsigned int)model->timeTillPumpAutostopSec);
            _display->draw2CTableViewLine(3, 48, "Stops in:", buff);
        }
    }
}

const char * WaterPumpView::textualizeConnectionState(WaterPumpConnectionState connectionState){
    switch (connectionState)
    {
        case WPCONNECTION_NONE:
            return "None";
        case WPCONNECTION_CONNECTING:
            return "Connecting...";
        case WPCONNECTION_CONNECTED:
            return "Connected";
        case WPCONNECTION_CONNECTEDBADSYSIGNAL:
            return "Noise Only";
        case WPCONNECTION_NOSIGNAL:
            return "No Signal";
        case WPCONNECTION_NORESPONSE:
            return "No Response";
        case WPCONNECTION_RFERROR:
            return "RF ERROR";
        default:
            break;
    }
    return nullptr;
}