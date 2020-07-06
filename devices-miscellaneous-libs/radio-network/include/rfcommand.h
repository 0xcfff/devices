#ifndef _RFCOMMAND_H_
#define _RFCOMMAND_H_

#include <stdint.h>

#define RFCOMMAND_ACK               0x10
#define RFCOMMAND_P2PCONNECT        0x20
#define RFCOMMAND_P2PACCEPT         0x21
#define RFCOMMAND_P2PREJECT         0x22
#define RFCOMMAND_P2PDISCONNECT     0x23
#define RFCOMMAND_PING              0x40
#define RFCOMMAND_PONG              0x41

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