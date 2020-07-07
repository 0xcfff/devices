#ifndef _RFCOMMAND_H_
#define _RFCOMMAND_H_

#include <stdint.h>

#define RFCOMMAND_NONE              0x00
#define RFCOMMAND_ACK               0x01
#define RFCOMMAND_NACK              0x02

#define RFCOMMAND_PING              0x0E
#define RFCOMMAND_PONG              0x0F

#define RFCOMMAND_P2PCONNECT        0x10
#define RFCOMMAND_P2PACCEPT         0x11
#define RFCOMMAND_P2PREJECT         0x12
#define RFCOMMAND_P2PDISCONNECT     0x13
#define RFCOMMAND_P2PHEARTBEAT      0x14

/**
 * Struct that defined RF Command header object. 
 * This object goes first in RFFrame and prepends RF command body.
*/
struct RFCommandHeader{
    /**
     * RF command
    */
    uint8_t command;
};

struct P2PConnectRFCommand {
    uint64_t replyAddress;
};

struct P2PAcceptRFCommand {
    uint64_t replyAddress;
};

struct P2PRejectRFCommand {
    uint8_t rejectReason;
};


#endif  // _RFCOMMAND_H_