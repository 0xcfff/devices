#include <macro-logs.h>

#include "waterpump-view.h"

WaterPumpView::WaterPumpView(Display * view):
    _display(view)
{
}

void WaterPumpView::drawModel(WaterPumpModel * model){
    const int buffSize = 50;
    char buff[buffSize];
    _display->drawTextLine(0, "Water Pump");

    snprintf(buff, buffSize, "Line: %s", textualizeConnectionState(model->connectionState));
    _display->drawTextLine(1, buff);

    if (model->connectionState == WPCONNECTION_CONNECTEDGOODSIGNAL || model->connectionState == WPCONNECTION_CONNECTEDBADSYSIGNAL)
    {

        if (!model->pumpIsWorking) {
            _display->drawTextLine(2, "State: Stopped");
        } else {
            uint16_t secPassed = (uint16_t)((millis() - model->lastStatusUpdatedMillis) / 1000);
            uint16_t secLeftTotal = model->timeTillPumpAutostopSec - secPassed;
            uint16_t minLeft = secLeftTotal / 60;
            uint16_t secLeft = secLeftTotal % 60;

            if (minLeft > 2) {
                snprintf(buff, buffSize, "Working (%i min)", (int)minLeft);
            } else if (minLeft <= 2 && minLeft >= 1) {
                snprintf(buff, buffSize, "Working (%im %is)", (int)minLeft, (int)secLeft);
            } else {
                snprintf(buff, buffSize, "Working (%i sec)", (int)secLeft);
            }
            _display->drawTextLine(2, buff);
            LOG_DEBUGF("Sec left %i\n", (int)model->timeTillPumpAutostopSec);
            LOG_DEBUGF("Sec passed %i\n", (int)secPassed);
        }

    }
    const char * selectedAction = textualizeWaterPumpAction(model->selectedAction);
    _display->drawTextLine(3, selectedAction);
}

const char * WaterPumpView::textualizeConnectionState(WaterPumpConnectionState connectionState){
    switch (connectionState)
    {
        case WPCONNECTION_NONE:
            return "None";
        case WPCONNECTION_CONNECTING:
            return "Connecting...";
        case WPCONNECTION_CONNECTEDGOODSIGNAL:
            return "Good Signal";
        case WPCONNECTION_CONNECTEDBADSYSIGNAL:
            return "Bad Signal";
        case WPCONNECTION_NOCONNECTION:
            return "No Signal";
        case WPCONNECTION_RFERROR:
            return "RF ERROR";
        default:
            break;
    }
    return nullptr;
}

const char * WaterPumpView::textualizeWaterPumpAction(WaterPumpAction action){
    switch (action)
    {
        case WPACTION_STOP:
            return "Stop";
        case WPACTION_ADDMAX:
            return "Add MAX";
        case WPACTION_ADD10MIN:
            return "Add 10 min";
        case WPACTION_ADD5MIN:
            return "Add 5 min";
        case WPACTION_ADD1MIN:
            return "Add 1 min";
        default:
            break;
    }
    return "Unknown";
}