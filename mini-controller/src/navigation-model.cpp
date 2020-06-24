#include "navigation-model.h"

NavigationModel::NavigationModel() :
    _defaultTargetIndex(NAVMODEL_NO_MODECTL_SELECTED),
    _currentTargetIndex(NAVMODEL_NO_MODECTL_SELECTED)
{

}

uint8_t NavigationModel::getCurrentNavItemIndex(){
    if (_currentTargetIndex == NAVMODEL_NO_MODECTL_SELECTED)
        return _defaultTargetIndex;
    return _currentTargetIndex;
}

NavigationTargetDescriptor * NavigationModel::getCurrentNavItemDescriptor() {
    uint8_t currentIndex = getCurrentNavItemIndex();
    if (currentIndex == NAVMODEL_NO_MODECTL_SELECTED)
        return nullptr;
    return _targets.at(currentIndex);
}

bool NavigationModel::navigateNextNavItem() {
    uint8_t currentIndex = getCurrentNavItemIndex();
    if (currentIndex == NAVMODEL_NO_MODECTL_SELECTED){
        return false;
    }

    uint8_t nextIndex = currentIndex + 1;
    if (nextIndex >= _targets.size()) {
        nextIndex = 0;
    }

    _currentTargetIndex = nextIndex;
    return _currentTargetIndex != currentIndex;
}

void NavigationModel::reset(){
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