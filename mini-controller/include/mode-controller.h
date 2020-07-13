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
    PROCESSOR_RESULT_NONE               = 0,
    PROCESSOR_RESULT_SUCCESS            = 1,
    PROCESSOR_RESULT_LEAVESTATE         = 3,
    PROCESSOR_RESULT_ENTERSTATE         = 4,
    PROCESSOR_RESULT_RISEEVENT          = 5
};
enum ModeControllerSystemEvent : uint8_t {
    PROCESSOR_EVENT_NONE                = 0,
    PROCESSOR_EVENT_SLEEP               = 1,
    PROCESSOR_EVENT_WAKEUP              = 2
};

enum ModeControllerSystemEventResult : uint8_t {
    PROCESSOR_EVENTRESULT_NONE          = 0,
    PROCESSOR_EVENTRESULT_HANDLED       = 1,
    PROCESSOR_EVENTRESULT_POSTPONE      = 2,
    PROCESSOR_EVENTRESULT_ERROR         = 3
};

struct ModeControllerHandleUserInputResultData {
    public:
        ModeControllerHandleUserInputResultData() {};
        ModeControllerHandleUserInputResultData(ModeControllerHandleUserInputResult result) : handleResult(result), riseEvent(PROCESSOR_EVENT_NONE) {};
        ModeControllerHandleUserInputResultData(ModeControllerSystemEvent event) : handleResult(PROCESSOR_RESULT_RISEEVENT), riseEvent(event) {};

    public:
        ModeControllerHandleUserInputResult handleResult;
        ModeControllerSystemEvent riseEvent;
};



class ModeController{
    public:

        virtual bool activate() = 0;
        virtual bool deactivate() = 0;

        virtual bool handleTick() = 0;
        // TODO: reconsider return value. It does not make sense to initiate sleep from this function
        virtual ModeControllerHandleUserInputResultData handleUserInput(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state) = 0;
        virtual ModeControllerSystemEventResult handleSystemEvent(ModeControllerSystemEvent systemEvent) = 0;
};

#endif  // _MODE_CONTROLLER_H_