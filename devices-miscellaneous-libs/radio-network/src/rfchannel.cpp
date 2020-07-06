#include <macro-flags.h>
#include <macro-logs.h>

#include "rfchannel.h"

RFChannel::RFChannel(RF24 * radio, size_t maxDataSize, uint16_t receiveTimeoutMsec) :
    _radio(radio),
    _maxDataSize(maxDataSize),
    _receiveTimeoutMsec(receiveTimeoutMsec),
    _allRecevingPipes((size_t)RFCHANNELCONST_MAXPIPES),
    _stateFlags(RFCHANNELSTATE_EMPTY)
{
    for (uint8_t pipe = 0; pipe < RFCHANNELCONST_MAXPIPES; pipe++)
    {
        _allRecevingPipes[pipe] = nullptr;
    }
}

bool RFChannel::handle() {
    handleReceive();
}

bool RFChannel::handleReceive(){
    bool received = false;
    uint8_t pipe;
    while (_radio->available(&pipe))
    {
        received |= (onReceive(pipe) == FRRECEIVERPIPE_RECEIVERESULT_RECEIVECOMPLETE);
    }
    
    return received;
}

bool RFChannel::handleReceiveTimeouts()
{
    bool timeoutsFound;
    auto currentMillis = millis();
    for (uint8_t pipe = 0; pipe < RFCHANNELCONST_MAXPIPES; pipe++)
    {
        ReceiverPipeInfo * pipeInfo = _allRecevingPipes.at(pipe);
        if (pipeInfo != nullptr && pipeInfo->state == FRRECEIVERPIPE_STATE_RECEIVING
            && (currentMillis - pipeInfo->lastReceivedMsec) > _receiveTimeoutMsec) {
                pipeInfo->state = FRRECEIVERPIPE_STATE_TIMEDOUT;
                timeoutsFound = true;
                break;
            }
    }
    return timeoutsFound;
}

RFChannel::ReceiveResult RFChannel::onReceive(uint8_t pipe)
{
    ReceiveResult received = FRRECEIVERPIPE_RECEIVERESULT_NONE;
    size_t payloadSize = _radio->getDynamicPayloadSize();
    if (payloadSize > 0 && payloadSize < RFFRAME_MAXSIZE) {
        ReceiverPipeInfo * pipeInfo = _allRecevingPipes.at(pipe);
        uint8_t buffer[payloadSize];
        RFFrameHeader frameHeader;
        _radio->read(buffer, payloadSize);
        
        // read signal level
        RFChannelPipeSignalLevel signalLevel = RFCHANNELSIGNALLEVEL_UNKNOWN;
        if (!_radio->testCarrier()) {
            if (!_radio->testRPD()) {
                signalLevel = RFCHANNELSIGNALLEVEL_GOOD;
            } else {
                signalLevel = RFCHANNELSIGNALLEVEL_BAD;
            }
        }

        size_t frameHeaderSize = decodeRFHeader(buffer, payloadSize, &frameHeader);
        size_t frameBodySize = payloadSize - frameHeaderSize;
        if (frameHeaderSize == 0) {
            LOG_DEBUGF("Unknown message of %i bytes received on pipe %i with local address %llX", (int)payloadSize, (int)pipe, pipeInfo->localAddress);
        } else {
            switch (pipeInfo->state)
            {
                case FRRECEIVERPIPE_STATE_INACTIVE:
                    LOG_DEBUGF("RFFrame received while pipe %i is in inactive state!", (int)pipe);
                    received = FRRECEIVERPIPE_RECEIVERESULT_SKIPPED;
                    break;
                case FRRECEIVERPIPE_STATE_IDLE:
                case FRRECEIVERPIPE_STATE_TIMEDOUT:
                    if (!IS_FLAG_SET(RFFRAME_FLAG_SEQFIRSTFRAME, frameHeader.flags)) {
                        LOG_DEBUGF("%i-th RFFrame received, expected first frame, pipe %i. Skipping.", (int)frameHeader.sequenceNumber, (int)pipe);
                        received = FRRECEIVERPIPE_RECEIVERESULT_SKIPPED;
                        break;
                    }
                    if (frameBodySize > _maxDataSize) {
                        LOG_DEBUGF("Received RF message is too big %i bytes, pipe %i. Skipping.", (int)frameBodySize, (int)pipe);
                        received = FRRECEIVERPIPE_RECEIVERESULT_SKIPPED;
                        break;
                    }
                    pipeInfo->state = FRRECEIVERPIPE_STATE_RECEIVING;
                    pipeInfo->firstHeader = frameHeader;
                    
                    memcpy(pipeInfo->contentBuffer, buffer+frameHeaderSize, frameBodySize);
                    pipeInfo->receivedContentSize += frameBodySize;
                    pipeInfo->lastReceivedFrameSeqenceNumber = frameHeader.sequenceNumber;
                    pipeInfo->lastReceivedMsec = millis();
                    pipeInfo->lastSignalLevel = signalLevel;
                    received = FRRECEIVERPIPE_RECEIVERESULT_PARTIAL;

                    if (IS_FLAG_SET(RFFRAME_FLAG_SEQLASTFRAME, frameHeader.flags)){
                        pipeInfo->state = FRRECEIVERPIPE_STATE_RECEIVED;
                        received = FRRECEIVERPIPE_RECEIVERESULT_RECEIVECOMPLETE;                
                    }
                    break;
                case FRRECEIVERPIPE_STATE_RECEIVING:
                    if (frameHeader.sequenceId != pipeInfo->firstHeader.sequenceId
                        || frameHeader.sequenceNumber != pipeInfo->lastReceivedFrameSeqenceNumber + 1) {
                            LOG_DEBUGF("%i-th RFFrame from %i sequence received, expected %i-th frame from %i sequence, pipe %i. Skipping.", 
                                (int)frameHeader.sequenceNumber, 
                                (int)frameHeader.sequenceId, 
                                (int)pipeInfo->lastReceivedFrameSeqenceNumber + 1, 
                                (int)pipeInfo->firstHeader.sequenceId,
                                (int)pipe
                                );
                        received = FRRECEIVERPIPE_RECEIVERESULT_SKIPPED;
                        break;                            
                    }
                    if (frameBodySize + pipeInfo->receivedContentSize > _maxDataSize) {
                        LOG_DEBUGF("Received RF message is too big %i bytes, pipe %i. Skipping.", (int)frameBodySize, (int)pipe);
                        received = FRRECEIVERPIPE_RECEIVERESULT_SKIPPED;
                        break;
                    }

                    memcpy(pipeInfo->contentBuffer + pipeInfo->receivedContentSize, buffer+frameHeaderSize, frameBodySize);
                    pipeInfo->receivedContentSize += frameBodySize;
                    pipeInfo->lastReceivedFrameSeqenceNumber = frameHeader.sequenceNumber;
                    pipeInfo->lastReceivedMsec = millis();
                    pipeInfo->lastSignalLevel = signalLevel;
                    received = FRRECEIVERPIPE_RECEIVERESULT_PARTIAL;
                    if (IS_FLAG_SET(RFFRAME_FLAG_SEQLASTFRAME, frameHeader.flags)){
                        pipeInfo->state = FRRECEIVERPIPE_STATE_RECEIVED;
                        received = FRRECEIVERPIPE_RECEIVERESULT_RECEIVECOMPLETE;                
                    }
                    break;
                case FRRECEIVERPIPE_STATE_RECEIVED:
                    LOG_DEBUGF("RFFrame received while pipe %i already has data!", (int)pipe);
                    received = FRRECEIVERPIPE_RECEIVERESULT_SKIPPED;
                    break;
                default:
                    break;
            }
        }
        return received;
    }
}

bool RFChannel::sendFrame(RFFrameHeader * frameHeader, void * frameData, size_t dataSize, bool broadcast)
{
    if (frameHeader == nullptr) {
        return false;
    }

    size_t headerSize = getRFHeaderEncodedSize(frameHeader);
    if (headerSize + dataSize > RFFRAME_MAXSIZE) {
        return false;
    }

    size_t buffSize = headerSize + dataSize;
    uint8_t buff[buffSize];
    encodeRFHeader(buff, buffSize, frameHeader);
    memcpy(buff+headerSize, frameData, dataSize);

    bool isListening = IS_FLAG_SET(RFCHANNELSTATE_LISTENING, _stateFlags);
    if (isListening) {
        _radio->stopListening();
    }
    _radio->openWritingPipe(frameHeader->toAddress);
    bool result = _radio->write(buff, buffSize, broadcast);
    if (isListening) {
        _radio->startListening();
    }
    return result;
}

bool RFChannel::sendData(uint64_t localAddress, uint64_t remoteAddress, uint8_t inResponseTo, void * data, size_t dataSize, bool broadcast){

}

bool RFChannel::sendCommand(uint64_t localAddress, uint64_t remoteAddress, uint8_t inResponseTo, uint8_t command, void * commandData, size_t dataSize, bool broadcast){

}

bool RFChannel::openRedingPipe(uint8_t pipe, uint64_t localAddress)
{
    if (isReadingPipeInUse(pipe)) {
        return false;
    }

    ReceiverPipeInfo * pipeInfo = _allRecevingPipes.at(pipe);
    if (pipeInfo == nullptr) {
        pipeInfo = new ReceiverPipeInfo();
        pipeInfo->contentBuffer = new uint8_t[_maxDataSize];
        _allRecevingPipes[pipe] = pipeInfo;
    }

    pipeInfo->state = FRRECEIVERPIPE_STATE_IDLE;
    _radio->openReadingPipe(pipe, localAddress);
    return true;
}

bool RFChannel::closeReadingPipe(uint8_t pipe){
    if (!isReadingPipeInUse(pipe)) {
        return false;
    }

    ReceiverPipeInfo * pipeInfo = _allRecevingPipes.at(pipe);
    _radio->closeReadingPipe(pipe);
    pipeInfo->state = FRRECEIVERPIPE_STATE_INACTIVE;
    return true;
}

bool RFChannel::isReadingPipeInUse(uint8_t pipe){
    if (pipe >= RFCHANNELCONST_MAXPIPES) {
        return false;
    }

    ReceiverPipeInfo * pipeInfo = _allRecevingPipes.at(pipe);
    return pipeInfo != nullptr && pipeInfo->state != FRRECEIVERPIPE_STATE_INACTIVE;
}

uint64_t RFChannel::getReadingPipeAddress(uint8_t pipe){
    if (!isReadingPipeInUse(pipe)) {
        return RFCHANNELCONST_NULLADDRESS;
    }
    ReceiverPipeInfo * pipeInfo = _allRecevingPipes.at(pipe);
    return pipeInfo->localAddress;
}

RFChannelContent RFChannel::getContentAvailable() 
{
    RFChannelContent content = RFCHANNEL_CONTENT_NONE;
    for (uint8_t pipe = 0; pipe < RFCHANNELCONST_MAXPIPES; pipe++)
    {
        ReceiverPipeInfo * pipeInfo = _allRecevingPipes.at(pipe);
        if (pipeInfo != nullptr && pipeInfo->state == FRRECEIVERPIPE_STATE_RECEIVED)
        {
            RFChannelContent pipeContent = IS_FLAG_SET(RFFRAME_FLAG_COMMAND, pipeInfo->firstHeader.flags) 
                ? RFCHANNEL_CONTENT_COMMAND
                : RFCHANNEL_CONTENT_DATA;
            content = (RFChannelContent)(content | pipeContent);
        }
    }
    return content;
}

RFChannelContent RFChannel::getContentAvailable(uint8_t pipe)
{
    RFChannelContent content = RFCHANNEL_CONTENT_NONE;
    ReceiverPipeInfo * pipeInfo = _allRecevingPipes.at(pipe);
    if (pipeInfo != nullptr && pipeInfo->state == FRRECEIVERPIPE_STATE_RECEIVED) {
        content = (IS_FLAG_SET(RFFRAME_FLAG_COMMAND, pipeInfo->firstHeader.flags) 
            ? RFCHANNEL_CONTENT_COMMAND
            : RFCHANNEL_CONTENT_DATA);
    }
    return content;
}

size_t RFChannel::getContentSize(uint8_t pipe)
{
    size_t contentSize = 0;
    ReceiverPipeInfo * pipeInfo = _allRecevingPipes.at(pipe);
    if (pipeInfo != nullptr && pipeInfo->state == FRRECEIVERPIPE_STATE_RECEIVED) {
        contentSize = pipeInfo->receivedContentSize;
    }
    return contentSize;
}

bool RFChannel::readHeader(uint8_t pipe, RFFrameHeader * header)
{
    bool success = false;    
    ReceiverPipeInfo * pipeInfo = _allRecevingPipes.at(pipe);
    if (pipeInfo != nullptr && pipeInfo->state == FRRECEIVERPIPE_STATE_RECEIVED) {
        memcpy(header, &pipeInfo->firstHeader, sizeof(RFFrameHeader));
        success = true;
    }
    return success;
}

bool RFChannel::receiveContent(uint8_t pipe, void * buff, size_t size)
{
    bool success = false;    
    ReceiverPipeInfo * pipeInfo = _allRecevingPipes.at(pipe);
    if (pipeInfo != nullptr && pipeInfo->state == FRRECEIVERPIPE_STATE_RECEIVED) {
        if (size >= pipeInfo->receivedContentSize) {
            memcpy(buff, pipeInfo->contentBuffer, pipeInfo->receivedContentSize);
            pipeInfo->state = FRRECEIVERPIPE_STATE_IDLE;
            success = true;
        }
    }
    return success;
}
