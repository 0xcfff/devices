#include "navigation-model.h"

NavigationModel::NavigationModel() :
    _defaultTargetIndex(0),
    _currentTargetIndex(0)
{

}

void NavigationModel::reset(){
    _currentTarget = nullptr;
    _currentTargetIndex = _defaultTargetIndex;
    _isTargetEntered = false;
}

void NavigationModel::clear(){
    _targets.clear();
    reset();
}


uint8_t NavigationModel::addTargetDescriptor(NavigationTargetDescriptor * mode){
    _targets.push_back(mode);
    uint8_t index = (uint8_t)_targets.size() - 1;
    if ((mode->flags & MODEDESCR_FLAG_DEFAULTMODE) == MODEDESCR_FLAG_DEFAULTMODE) {
        _defaultTargetIndex = index;
    }

    return index;
}