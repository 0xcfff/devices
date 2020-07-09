#include "waterpump-model.h"

void WaterPumpModel::clear(){
    connectionState = WPCONNECTION_NONE;
    selectedAction = WPACTION_DEFAULTACTION;
    pumpIsWorking = false;
    lastStatusUpdatedMillis = 0;
    timeSincePumpStartedSec = 0;
    timeTillPumpAutostopSec = 0;
}

WaterPumpAction WaterPumpModel::selectNextAction(){
    int currAction = (int)selectedAction;
    currAction += 1;
    if (currAction > (int)WPACTION_MAXACTION) {
        currAction = 0;
    }
    selectedAction = (WaterPumpAction)currAction;
    return selectedAction;
}