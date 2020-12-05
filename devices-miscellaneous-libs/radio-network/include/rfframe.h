#ifndef _RFFRAME_H_
#define _RFFRAME_H_

#include <stddef.h>
#include <stdint.h>

/**
 * Maximum size of RF frame including all the headers
*/
#define RFFRAME_MAXSIZE             32

/**
 * Indicates that the current frame or sequence of frames
 * is the response to previously received sequence
*/
#define RFFRAME_FLAG_REPLY          0x80

/**
 * Indicates that the current frame is the first frame 
 * in the sequence of frames reprecenting single piece of data.
 * Note that if both RFFRAME_FLAG_SEQFIRSTFRAME and RFFRAME_FLAG_SEQLASTFRAME 
 * are specified, then the current frame will be the only frame in the sequence
*/
#define RFFRAME_FLAG_SEQFIRSTFRAME  0x40

/**
 * Indicates that the current frame is the first frame 
 * in the sequence of frames reprecenting single piece of data
 * Note that if both RFFRAME_FLAG_SEQFIRSTFRAME and RFFRAME_FLAG_SEQLASTFRAME 
 * are specified, then the current frame will be the only frame in the sequence
*/
#define RFFRAME_FLAG_SEQLASTFRAME   0x20

/**
 * If flag is specified, the frame contains network level command, 
 * otherwice it contains data.
*/
#define RFFRAME_FLAG_COMMAND        0x10

/**
 * If the flag is specified, receiving side should send acknowledge 
 * confirming that each frame from this sequence was received.
*/
#define RFFRAME_FLAG_REQUIRESACK    0x08

/**
 * If the flag is specified, extra flags byte is sent
*/
#define RFFRAME_FLAG_EXTRAFLAGS     0x01

/* 14 bytes without length
IP Like Header
  Version : 4 bit
  Flags : 4 bit 
  Protocol: 8
  Length : 8 ???
  Identification (sequenceId, sequenceNumber) : 16
  Source IP : 40
  Dest IP : 40
*/

struct RFFrameHeader{
    /**
     * Contains flags the content of the frame and its header
    */
    uint8_t flags;

    /**
     * Additional extra flags if needed
    */
    uint8_t additionalFlags;

    /**
     * Unique Id for all messages sent in scope of this frame sequence
    */
    uint8_t sequenceId;

    /**
     * Unique sequential number of the frame in the sequence of frames. 
     * First frame in a sequence should have sequenceNumber equal to 0
     */
    uint8_t sequenceNumber;

    /**
     * Contains ID of sequence to whcih this reply frame 
     * or sequenceof frames are sent
    */
    uint8_t inReplyToSequenceId;

    /**
     * 5 bytes address of the sender. 
     * This field is only sent in the first message of a sequence.
    */
    uint64_t fromAddress;

    /**
     * 5 bytes address of the receiver
     * This field is only sent in the first message of a sequence.
    */
    uint64_t toAddress;
};

/**
 * Returns size of the encoded RF frame header in bytes
 * @param  {RFFrameHeader*} header : RF frame header
 * @return {uint8_t}               : size of encoded RF frame header in bytes
 */
extern size_t getRFHeaderEncodedSize(RFFrameHeader * header);

/**
 * Encodes RF header to specified buffer
 * @param  {void*} buff            : buffer
 * @param  {size_t} buffSize       : buffer size
 * @param  {RFFrameHeader*} header : RF frame header
 * @return {uint8_t}               : number of bytes RF header consumes in encoded state
 */
extern size_t encodeRFHeader(void * buff, size_t buffSize, RFFrameHeader * header);

/**
 * Decodes RF header from specified buffer
 * @param  {void*} buff            : buffer
 * @param  {size_t} buffSize       : buffer size
 * @param  {RFFrameHeader*} header : RF frame header
 * @return {uint8_t}               : number of bytes RF header consumed in encoded state
 */
extern size_t decodeRFHeader(void * buff, size_t buffSize, RFFrameHeader * header);


#endif  // _RFFRAME_H_