#include <macro-logs.h>
#include <macro-flags.h>

#include "ota-updater.h"

OtaUpdater::OtaUpdater():
    _stateFlags(OTASTATE_EMPTY),
    _status(OTAUPDATERSTATUS_NONE),
    _progress(0),
    _lastActivityAt(0)
{    
}


bool OtaUpdater::begin(){
    return init();
}

bool OtaUpdater::init() {
  if (!IS_FLAG_SET(OTASTATE_ACTIVE, _stateFlags)) {
    ArduinoOTA.onStart(std::bind(&OtaUpdater::onStartOTA, this));
    ArduinoOTA.onError(std::bind(&OtaUpdater::onErrorOTA, this, std::placeholders::_1));
    ArduinoOTA.onProgress(std::bind(&OtaUpdater::onProgressOTA, this, std::placeholders::_1, std::placeholders::_2));
    ArduinoOTA.onEnd(std::bind(&OtaUpdater::onEndOTA, this));
    _status = OTAUPDATERSTATUS_DISABLED;
    _progress = 0;
    SET_FLAG(OTASTATE_ACTIVE, _stateFlags);
  }
  return true;
}


bool OtaUpdater::end() {
    bool result = true;
    if (IS_FLAG_SET(OTASTATE_ON, _stateFlags)){
        result = enableOta(false);
    }
    if (result && IS_FLAG_SET(OTASTATE_ACTIVE, _stateFlags)) {
        RESET_FLAG(OTASTATE_ACTIVE, _stateFlags);
    }
    return result;
}

bool OtaUpdater::handle() {
    if (IS_FLAG_SET(OTASTATE_ON, _stateFlags)) {
        ArduinoOTA.handle();
    }
    return true;
}

bool OtaUpdater::isEnabled() {
    return IS_FLAG_SET(OTASTATE_ON, _stateFlags);
}

OtaUpdaterStatus OtaUpdater::getStatus(){
    return _status;
}

uint8_t OtaUpdater::getProgress(){
    return _progress;
}

unsigned long OtaUpdater::getLastActivityAt(){
    unsigned long result = 0;
    if (IS_FLAG_SET(OTASTATE_ON, _stateFlags)){
        result = _lastActivityAt;
    }
    return result;
}

bool OtaUpdater::enableOta(bool enabled){
    bool result = false;
    if (IS_FLAG_SET(OTASTATE_ACTIVE, _stateFlags))
    {
        if (enabled) {
            ArduinoOTA.begin();
            _lastActivityAt = millis();
            _status = OTAUPDATERSTATUS_WAITINGCONNECTION;
            _progress = 0;
            SET_FLAG(OTASTATE_ON, _stateFlags);
        } else {
            _lastActivityAt = 0;
            _status = OTAUPDATERSTATUS_DISABLED;
            _progress = 0;
            RESET_FLAG(OTASTATE_ON, _stateFlags);
        }
        result = true;
    }
    return result;
}


void OtaUpdater::onStartOTA(){
    _status = OTAUPDATERSTATUS_INPROGRESS;
    _progress = 0;
    _lastActivityAt = millis();
}

void OtaUpdater::onErrorOTA(ota_error_t error){
    _status = OTAUPDATERSTATUS_ERROR;
    const char * errorDescr = NULL;
    switch (error)
    {
        case OTA_AUTH_ERROR:
            errorDescr = "auth";
            break;
        case OTA_BEGIN_ERROR:
            errorDescr = "begin";
            break;
        case OTA_END_ERROR:
            errorDescr = "end";
            break;
        case OTA_RECEIVE_ERROR:
            errorDescr = "receive";
            break;
        default:
            errorDescr = "unknown";
            break;
    }
    LOG_ERRORF("OTA error: %s\n", errorDescr);
}

void OtaUpdater::onProgressOTA(unsigned int progress, unsigned int total){
    int progressPercent = (int)(progress * (100 / (float)total));
    LOG_INFOF("OTA progress: %i%%\n", progressPercent);
    _progress = (uint8_t)progressPercent;
    _lastActivityAt = millis();
}

void OtaUpdater::onEndOTA(){
    _status = OTAUPDATERSTATUS_COMPLETED;
    _progress = 100;
    _lastActivityAt = millis();
    LOG_INFOLN("OTA completed, restarting...");
    ESP.restart();
}