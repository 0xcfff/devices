#ifndef _RFADDRESS_H_
#define _RFADDRESS_H_

#include <stdint.h>

/**
 * RF Pipe has 5 bytes address
 * 4 first bytes are used to store IP adress of a device in IP notation
 * 1 last byte is used to indicate slot that is used to establish connections 
 * with other RF transmitters that want to communicate with this RF transmitter 
 * using IP-like addressing.
 * Consider following scenario, there is a device that we want to associate with static IP:
 * IP: 10.200.85.25
 * CIDR: 255.255.255.0
 * Address can be also expressed as 10.200.85.25/24
 * Hex representation:
 * 0x10C85519 - hexadecimal representation of the IP
 * 0x10C85500 - hexadecimal representation of the network address
 * Now, following RF level addresses are used:
 * 0x10C855FFFF - should be listened by pipe0 when no transmittion activity is in progress 
 * to catch network level broadcast messages
 * 0x10C8551900 - should be listened by pipe1 in broadcast mode ie any other RF device
 * can send a broadcast (no RF-level confirmation) message to this address.
 * Same address should be used by pipe0 when sending initial messages to other RF devices.
 * 0x10C8551901 - 0x10C85519FF - should be listened by pipe2-pipe5. These adresses are used 
 * to establish P2P connections with guaranteed message delivery on RF level.
*/

#endif  // _RFADDRESS_H_