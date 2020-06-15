#ifndef _IDLE_PROCESSOR_H_
#define _IDLE_PROCESSOR_H_

#include "main-view.h"

class IdleProcessor{
    public:
        IdleProcessor(MainView & view);
    
        bool activate();

    private:
        MainView & _view;

};

#endif // _IDLE_PROCESSOR_H_