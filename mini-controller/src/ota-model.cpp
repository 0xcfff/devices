#include "ota-model.h"

OtaModel::OtaModel(){
    clear();
}

void OtaModel::clear(){
    otaStatus = OTAUPDATERSTATUS_NONE;
    otaProgress = 0;
    statusText = nullptr;
}

