#include "idle-processor.h"
#include "Sleep.xbm"

IdleProcessor::IdleProcessor(Display & view):
    _view(view)
{

}

bool IdleProcessor::activate(){

    _view.drawImageView(Sleep_width, Sleep_height, Sleep_bits, "Баиньки...");
    
}