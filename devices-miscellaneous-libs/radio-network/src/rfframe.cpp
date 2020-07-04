#include <cstring>

#include <macro-flags.h>

#include "rfframe.h"

uint8_t getRFHeaderEncodedSize(RFFrameHeader * header)
{
    uint8_t resultSize = 0;

    // flags
    resultSize += 1;

    // additionalFlags
    if (IS_FLAG_SET(RFFRAME_FLAG_EXTRAFLAGS, header->flags)) {
        resultSize += 1;
    }

    // sequenceId
    resultSize += 1;

    // sequenceNumber
    bool isSequence = false;
    if (!IS_FLAG_SET(RFFRAME_FLAG_SEQFIRSTFRAME, header->flags) || !IS_FLAG_SET(RFFRAME_FLAG_SEQLASTFRAME, header->flags)) {
        resultSize += 1;
        isSequence = true;
    }

    // inReplyToSequenceId
    if (IS_FLAG_SET(RFFRAME_FLAG_REPLY, header->flags)) {
        resultSize += 1;
    }

    // fromAddress and toAddress
    if (!isSequence || header->sequenceNumber == 0) {
        resultSize += 10;
    }

    return resultSize;
}

uint8_t encodeRFHeader(void * buff, size_t buffSize, RFFrameHeader * header)
{
    if (getRFHeaderEncodedSize(header) > buffSize){
        return 0;
    }

    uint8_t * byteStream = (uint8_t*)buff;
    uint8_t resultSize = 0;

    // flags
    *byteStream = header->flags;
    byteStream++;
    resultSize++;

    // additionalFlags
    if (IS_FLAG_SET(RFFRAME_FLAG_EXTRAFLAGS, header->flags)) {
        *byteStream = header->additionalFlags;
        byteStream++;
        resultSize += 1;
    }

    // sequenceId
    *byteStream = header->sequenceId;
    byteStream++;
    resultSize++;

    // sequenceNumber
    bool isSequence = false;
    if (!IS_FLAG_SET(RFFRAME_FLAG_SEQFIRSTFRAME, header->flags) || !IS_FLAG_SET(RFFRAME_FLAG_SEQLASTFRAME, header->flags)) {
        *byteStream = header->sequenceNumber;
        byteStream++;
        resultSize++;
        isSequence = true;
    }

    // inReplyToSequenceId
    if (IS_FLAG_SET(RFFRAME_FLAG_REPLY, header->flags)) {
        *byteStream = header->inReplyToSequenceId;
        byteStream++;
        resultSize++;
    }

    // fromAddress and toAddress
    if (!isSequence || header->sequenceNumber == 0) {
        for (int i = 4; i >= 0; i--) {

            *byteStream = ((header->fromAddress & ((uint64_t)0xff << (8 * i))) >> (8 * i));
            byteStream++;
            resultSize++;
        }

        for (int i = 4; i >= 0; i--) {

            *byteStream = ((header->toAddress & ((uint64_t)0xff << (8 * i))) >> (8 * i));
            byteStream++;
            resultSize++;
        }
    }

    return resultSize;
}

uint8_t decodeRFHeader(void * buff, size_t buffSize, RFFrameHeader * header){

    if (buffSize == 0) {
        return 0;
    }

    memset(header, 0, sizeof(RFFrameHeader));
    uint8_t * byteStream = (uint8_t*)buff;
    uint8_t resultSize = 0;

    // flags
    header->flags = *byteStream;
    byteStream++;
    resultSize++;

    // additionalFlags
    if (IS_FLAG_SET(RFFRAME_FLAG_EXTRAFLAGS, header->flags)) {
        if (resultSize + 1 > buffSize) {
            return 0;
        }

        header->additionalFlags = *byteStream;
        byteStream++;
        resultSize += 1;
    }

    // sequenceId
    if (resultSize + 1 > buffSize) {
        return 0;
    }
    header->sequenceId = *byteStream;
    byteStream++;
    resultSize++;

    // sequenceNumber
    bool isSequence = false;
    if (!IS_FLAG_SET(RFFRAME_FLAG_SEQFIRSTFRAME, header->flags) || !IS_FLAG_SET(RFFRAME_FLAG_SEQLASTFRAME, header->flags)) {
        if (resultSize + 1 > buffSize) {
            return 0;
        }
        header->sequenceNumber = *byteStream;
        byteStream++;
        resultSize++;
        isSequence = true;
    }

    // inReplyToSequenceId
    if (IS_FLAG_SET(RFFRAME_FLAG_REPLY, header->flags)) {
        if (resultSize + 1 > buffSize) {
            return 0;
        }
        header->inReplyToSequenceId = *byteStream;
        byteStream++;
        resultSize++;
    }

    // fromAddress and toAddress
    if (!isSequence || header->sequenceNumber == 0) {
        if (resultSize + 10 > buffSize) {
            return 0;
        }
        for (int i = 4; i >= 0; i--) {

            header->fromAddress |= ((uint64_t)(*byteStream) << (8 * i));
            byteStream++;
            resultSize++;
        }

        for (int i = 4; i >= 0; i--) {

            header->toAddress |= ((uint64_t)(*byteStream) << (8 * i));
            byteStream++;
            resultSize++;
        }
    }
    return resultSize;
}