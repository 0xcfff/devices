#ifndef _RFTRANSPORT_H_
#define _RFTRANSPORT_H_

#include <stdint.h>
#include <stddef.h>

#include <RF24.h>

#include "rfchannel.h"

enum RFSignalLevel{
    RADIOSIGNALLEVEL_UNKNOWN        = 0,
    RADIOSIGNALLEVEL_GOOD           = 1,
    RADIOSIGNALLEVEL_BAD            = 2,
    RADIOSIGNALLEVEL_NOSIGNAL       = 3
};

struct RFPingResult{
    bool success;
    RFSignalLevel signalLevel;
    uint16_t timeTaken;

    
    explicit operator bool() const { return success; }
};

class RFTransport{
    public:
        RFTransport(RF24 * radio, uint32_t ip, uint32_t networkCidr, uint16_t defaultTimeoutMsec);

        bool handle();

        RFPingResult ping(uint32_t ip, uint16_t timeout = 0);
        
        bool isConnectedP2P(uint32_t ip);
        bool connectP2P(uint32_t ip, uint16_t timeout = 0);
        bool disconnectP2P(uint32_t ip, uint16_t timeout = 0);
        std::vector<uint32_t> getActiveP2Pconnections();

        bool sendData(uint32_t ip, void * data, size_t size, uint16_t timeout = 0);
        bool sendDataAsync(uint32_t ip, void * data, size_t size, uint16_t timeout = 0);
        bool sendRequest(uint32_t ip, void * request, size_t requestSize, void * response, size_t responseSize, uint16_t timeout = 0);
        bool sendAsync(uint32_t ip, void * request, size_t requestSize, void * response, size_t responseSize, uint16_t timeout = 0);

        bool isDataReceived();
        uint32_t getReceivedSourceIP();
        uint32_t getReceivedTargetIP();
        size_t getReceivedDataSize();
        bool receiveData(void * buff, size_t size);

    private:
        RFChannel _channel;
        uint32_t _ip;
        uint8_t _networkBits;
        uint16_t _defaultTimeoutMsec;
};

#endif  // _RFTRANSPORT_H_