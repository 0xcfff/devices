#ifndef _BUTTONS_PROCESSOR_H_
#define _BUTTONS_PROCESSOR_H_

#include <stdint.h>

#include "idle-processor.h"

enum ButttonsProcessingResult : uint8_t {
    PROCESSOR_HANDLE_RESULT_SUCCESS     = 1,
    PROCESSOR_HANDLE_RESULT_ERROR       = 0,
    PROCESSOR_HANDLE_RESULT_IDLE        = 2
};


class ButtonsProcessor{

    public:
        ButtonsProcessor(uint8_t modePin, uint8_t confirmPin, uint8_t cancelPin, IdleProcessor & idleProcessor);

        ButttonsProcessingResult handle();

    private:

    private:
        uint8_t _modePin;
        uint8_t _confirmPin;
        uint8_t _cancelPin;
        IdleProcessor & _idleProcessor;
};


#endif // _BUTTONS_PROCESSOR_H_