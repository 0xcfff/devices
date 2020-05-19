#ifndef _INFO_COMMAND_HANDLER_H_
#define _INFO_COMMAND_HANDLER_H_

#include "pump_messages.h"

class InfoCommandHandler{
    public:
        InfoCommandHandler();
        PumpControlInfoResponse handle();
};

#endif