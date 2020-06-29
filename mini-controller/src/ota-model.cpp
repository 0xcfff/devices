#include "ota-model.h"

OtaModel::OtaModel(){
    clear();
}

void OtaModel::clear(){
    otaState = OTAMODELSTATE_NONE;
    otaProgress = 0;
    statusText = nullptr;
}

void OtaModel::setState(uint8_t newState, char * newStatusText){
    otaState = newState;
    statusText = newStatusText;
}
