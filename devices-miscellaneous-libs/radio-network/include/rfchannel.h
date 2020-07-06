#ifndef _RFCHANNEL_H_
#define _RFCHANNEL_H_

#include <vector>

#include <stdint.h>
#include <stddef.h>

#include <RF24.h>
#include "rfframe.h"

#define RFCHANNELSTATE_EMPTY        0x00
#define RFCHANNELSTATE_ACTIVE       0x01
#define RFCHANNELSTATE_LISTENING    0x02

#define RFCHANNELCONST_MAXPIPES     6
#define RFCHANNELCONST_NULLADDRESS  0xFFFFFFFFFFFFFFFF

enum RFChannelContent : uint8_t{
    RFCHANNEL_CONTENT_NONE      = 0x00,
    RFCHANNEL_CONTENT_COMMAND   = 0x01,
    RFCHANNEL_CONTENT_DATA      = 0x02,
};

enum RFChannelPipeSignalLevel : uint8_t{
    RFCHANNELSIGNALLEVEL_UNKNOWN   = 0,
    RFCHANNELSIGNALLEVEL_GOOD      = 1,
    RFCHANNELSIGNALLEVEL_BAD       = 2,
    RFCHANNELSIGNALLEVEL_NOSIGNAL  = 3
};


class RFChannel {
    private:
        enum ReceiveResult : uint8_t{
            FRRECEIVERPIPE_RECEIVERESULT_NONE,
            FRRECEIVERPIPE_RECEIVERESULT_PARTIAL,
            FRRECEIVERPIPE_RECEIVERESULT_RECEIVECOMPLETE,
            FRRECEIVERPIPE_RECEIVERESULT_SKIPPED
        };
        enum ReceiverPipeState : uint8_t{
            FRRECEIVERPIPE_STATE_INACTIVE       = 0x00,
            FRRECEIVERPIPE_STATE_IDLE           = 0x01,
            FRRECEIVERPIPE_STATE_RECEIVING      = 0x02,
            FRRECEIVERPIPE_STATE_RECEIVED       = 0x03,
            FRRECEIVERPIPE_STATE_TIMEDOUT       = 0x04
        };

        struct ReceiverPipeInfo{
            uint8_t * contentBuffer;
            size_t receivedContentSize;
            uint8_t lastReceivedFrameSeqenceNumber;
            uint64_t localAddress;
            RFFrameHeader firstHeader;
            ReceiverPipeState state;
            RFChannelPipeSignalLevel lastSignalLevel;
            unsigned long lastReceivedMsec;
        };

    public:
        RFChannel(RF24 * radio, size_t maxDataSize, uint16_t receiveTimeoutMsec);

        bool handle();

        bool sendFrame(RFFrameHeader * frameHeader, void * frameData, size_t dataSize, bool broadcast);
        bool sendData(uint64_t localAddress, uint64_t remoteAddress, uint8_t inResponseTo, void * data, size_t dataSize, bool broadcast);
        bool sendCommand(uint64_t localAddress, uint64_t remoteAddress, uint8_t inResponseTo, uint8_t command, void * commandData, size_t dataSize, bool broadcast);

        bool openRedingPipe(uint8_t pipe, uint64_t localAddress);
        bool closeReadingPipe(uint8_t pipe);        
        bool isReadingPipeInUse(uint8_t pipe);
        uint64_t getReadingPipeAddress(uint8_t pipe);
        
        RFChannelContent getContentAvailable();
        RFChannelContent getContentAvailable(uint8_t pipe);

        size_t getContentSize(uint8_t pipe);
        bool readHeader(uint8_t pipe, RFFrameHeader * header);
        bool receiveContent(uint8_t pipe, void * buff, size_t size);

    private:
        bool handleReceive();
        bool handleReceiveTimeouts();
        ReceiveResult onReceive(uint8_t pipe);

    private:
        RF24 * _radio;
        size_t _maxDataSize;
        uint16_t _receiveTimeoutMsec;
        std::vector<ReceiverPipeInfo*> _allRecevingPipes;
        uint8_t _stateFlags;
        
};

#endif  // _RFCHANNEL_H_