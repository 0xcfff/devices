#ifndef _DACHA1_NETWORK_H_
#define _DACHA1_NETWORK_H_

// Network address structure:
// Example: 0xCCCCCCC1C4LL
// - CCCCCC - l3 network address
// - C1     - subnet, C1 stends for water pump
// - C4     - address space
// see detailed description at http://tmrh20.blogspot.com/2016/08/rf24-addressing-review-of-nrf24l01.html
#define RF_NETWORK_DACHA1             0xCCCCCC0000LL
#define RF_DEVICEID_CONTROLLER        0xC0
#define RF_DEVICEID_WATERPUMP         0xC1
#define RF_DEVICEID_BANYA             0xC2
#define RF_DEVICEID_UNKNOWN4          0xC3
#define RF_DEVICEID_UNKNOWN5          0xC4

#define RF_L3NET_P2P_ADDR(network, sourceDeviceId, targetDeviceId) ((network & 0xFFFFFF0000LL) | ((0xFF & targetDeviceId) << 8) | (0xFF & sourceDeviceId))
#define RF_L3NET_CS_ADDR(network, targetDeviceId) ((network & 0xFFFFFF0000LL) | ((targetDeviceId && 0xFF) << 8) | (targetDeviceId && 0xFF))

#endif