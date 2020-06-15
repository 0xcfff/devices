#include "buttons-processor.h"

ButtonsProcessor::ButtonsProcessor(uint8_t modePin, uint8_t confirmPin, uint8_t cancelPin, IdleProcessor & idleProcessor) :
    _modePin(modePin),
    _confirmPin(confirmPin),
    _cancelPin(cancelPin),
    _idleProcessor(idleProcessor)
{

}


ButttonsProcessingResult ButtonsProcessor::handle(){

    _idleProcessor.activate();

    return PROCESSOR_HANDLE_RESULT_SUCCESS;
}