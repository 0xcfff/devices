#ifndef _IDLE_PROCESSOR_H_
#define _IDLE_PROCESSOR_H_

#include "display.h"

class IdleProcessor{
    public:
        IdleProcessor(Display & view);
    
        bool activate();

    private:
        Display & _view;

};

#endif // _IDLE_PROCESSOR_H_