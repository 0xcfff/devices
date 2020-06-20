#ifndef _NAVIGATION_MODEL_H_
#define _NAVIGATION_MODEL_H_

#include <stdint.h>

#include <vector>

#define MODEDESCR_FLAG_DEFAULTMODE          1
#define MODEDESCR_FLAG_USEXBMP              2
#define MODEDESCR_FLAG_USEPSTR              4

#define NAVMODEL_NO_MODECTL_SELECTED        0xFF

struct NavigationTargetDescriptor {
    const char * modeName;
    uint8_t flags;
    const uint8_t * splashScreenXBM;
    uint8_t splashWidth;
    uint8_t splashHeight;
};

class NavigationModel {
    public:
        NavigationModel();

        /**
         * Resets state of the model, but does not clear internal data
         * @return {void}
         */
        void reset();

        
        /**
         * Clears state of the model including internal data
         */
        void clear();

        
        /**
         * 
         * @param  {ModeDescription*} mode : Definition of the navigation item
         * @return {uint8_t}               : Index of the navigation item in the menu items list
         */
        uint8_t addTargetDescriptor(NavigationTargetDescriptor * mode);

    public:    

    private:
        std::vector<NavigationTargetDescriptor *> _targets;
        uint8_t _defaultTargetIndex;

        uint8_t _currentTargetIndex;
        NavigationTargetDescriptor * _currentTarget;
        bool _isTargetEntered;

};

#endif  // _NAVIGATION_MODEL_H_