#ifndef _MODE_CONTROLLER_H_
#define _MODE_CONTROLLER_H_

enum ModeControllerCommandButton : uint8_t {
    PROCESSOR_BUTTON_NONE     = 0x00,
    PROCESSOR_BUTTON_MENU     = 0x01,
    PROCESSOR_BUTTON_OK       = 0x02,
    PROCESSOR_BUTTON_CANCEL   = 0x04
};
enum ModeControllerCommandButtonAction : uint8_t {
    PROCESSOR_BUTTON_ACTION_NONE        = 0,
    PROCESSOR_BUTTON_ACTION_CLICK       = 1,
    PROCESSOR_BUTTON_ACTION_DBLCLICK    = 2,
    PROCESSOR_BUTTON_ACTION_LONGPRESS   = 3
};
enum ModeControllerHandleUserInputResult : uint8_t {
    PROCESSOR_RESULT_ERROR              = (uint8_t)-1,
    PROCESSOR_RESULT_SUCCESS            = 1,
    PROCESSOR_RESULT_UNKNOWN            = 2,
    PROCESSOR_RESULT_LEAVESTATE         = 3
};



class ModeController{
    public:

        virtual bool activate() = 0;
        virtual bool deactivate() = 0;

        virtual bool handleTick() = 0;
        virtual ModeControllerHandleUserInputResult handleUserInput(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state) = 0;
};

#endif  // _MODE_CONTROLLER_H_