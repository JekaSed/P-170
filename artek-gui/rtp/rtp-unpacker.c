#include "common.h"
#include "util.h"
#include "unpacker.h"
#include "unpacker_t.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#ifndef TRUE
#define TRUE (1)
#define FALSE (0)
#endif

//-------------------------------------------------------------------------------------------------
RtpUnpacker* RtpUnpacker_Create()
{
    RtpUnpacker* self = (RtpUnpacker*)malloc(sizeof(RtpUnpacker));
    RtpUnpacker_Init(self);
    return self;
}

//-------------------------------------------------------------------------------------------------
void RtpUnpacker_Destroy(RtpUnpacker* self)
{
    free(self);
}

//-------------------------------------------------------------------------------------------------
void RtpUnpacker_Init(RtpUnpacker* self)
{
    self->firstRunStatus = TRUE;
    self->flags = 0;
    self->timestampSkip = 0;
    self->sequenceSkip = 0;
    self->lastDataSize = 0;
    self->lastSeq = 0;
    memset(&self->header, 0, sizeof(RtpHeader));
}

//-------------------------------------------------------------------------------------------------
static void _CheckSequenceCounter(RtpUnpacker* self)
{
    uint16_t nextSeq = (self->lastSeq + 1) & 0xFFFF;
    self->sequenceSkip = 0;

    if (self->firstRunStatus || (nextSeq == self->header.sequenceNum)) {
        return;
    }

    self->flags |= RTP_EVENT_SEQUENCE_NUM_ERR;

    const int16_t dist = (int16_t)(nextSeq - self->header.sequenceNum);
    const uint32_t absDist = abs(dist);
    if (absDist <= RTP_MAX_DROP_SEQ) {
        if (dist < 0) {
            //были потеряны пакеты
            self->sequenceSkip = absDist;
        } else {
            //нарушение порядка
            self->flags |= RTP_EVENT_ORDER_ERR;
        }
    } else {
        //потеря синхронизации
        self->flags |= RTP_EVENT_UNSYNC_ERR;
    }
}

//-------------------------------------------------------------------------------------------------
static void _CheckTimestampCounter(RtpUnpacker* self)
{
    const uint32_t nextTm = (self->lastTimestamp + ((uint32_t)self->lastDataSize)) & 0xFFFFFFFF;
    self->timestampSkip = 0;

    if (self->firstRunStatus || (nextTm == self->header.timeStamp)) {
        return;
    }

    self->flags |= RTP_EVENT_TIMESTAMP_ERR;

    int dist = (int)(nextTm - self->header.timeStamp);
    if (dist < 0) {
        //имеет смысл только при потере пакетов
        self->timestampSkip = abs(dist);
    }
}

//-------------------------------------------------------------------------------------------------
static int _CheckHeader(RtpUnpacker* self, const uint8_t* datagram, const int size)
{
    uint8_t version = (datagram[0] >> 6) & 0b11;
    self->header.padding = (datagram[0] >> 5) & 0b1;

    //проверка RTP формата
    if (version != 2) {
        RtpUnpacker_Reset(self);
        self->flags |= RTP_EVENT_FORMAT_ERR;
        return -1;
    }

    self->header.marker = (datagram[1] >> 7) & 0b1;
    self->header.packetType = (datagram[1] & ((1 << 7) - 1));

    self->lastSeq = self->header.sequenceNum;
    self->lastTimestamp = self->header.timeStamp;
    uint8_t lastExMode = self->header.headerExtensionMode;
    self->header.headerExtensionMode = (datagram[0] >> 4) & 0b1;
    self->header.cc = datagram[0] & 0b1111;
    self->header.sequenceNum = fromBigEndian16(datagram + 2);
    self->header.timeStamp = fromBigEndian32(datagram + 4);
    self->header.ssrc = fromBigEndian32(datagram + 8);

    int datagramSize = RTP_DEFAULT_HEADER_SIZE + self->header.cc * sizeof(uint32_t);

    if (size < datagramSize) {
        RtpUnpacker_Reset(self);
        self->flags |= RTP_EVENT_FORMAT_ERR;
        return -RTP_EVENT_FORMAT_ERR;
    }

    int offset = 12;
    for (int i = 0; i < self->header.cc; ++i) {
        self->header.csrcs[i] = fromBigEndian32(datagram + offset);
        offset += sizeof(offset);
    }

    if (self->header.headerExtensionMode) {
        self->header.extensionProfileData = fromBigEndian16(datagram + offset);
        self->header.extensionSize = fromBigEndian16(datagram + offset + 2);
        datagramSize += (self->header.extensionSize + 1) * sizeof(uint32_t);

        if (datagramSize > size) {
            RtpUnpacker_Reset(self);
            self->flags |= RTP_EVENT_FORMAT_ERR;
            return -RTP_EVENT_FORMAT_ERR;
        }
        offset += sizeof(uint32_t);
        for (int i = 0; i < self->header.extensionSize; ++i) {
            self->header.extensionData[i] = fromBigEndian32(datagram + offset);
            offset += sizeof(uint32_t);
        }
    }

    const int headerSize = calcHeaderSize(&self->header);
    int paddingLen = 0;

    if (self->header.padding) {
        paddingLen = datagram[size - 1];
        if (paddingLen > size - 1 - headerSize) {
            RtpUnpacker_Reset(self);
            self->flags |= RTP_EVENT_FORMAT_ERR;
            return -RTP_EVENT_FORMAT_ERR;
        }
    }

    self->flags = 0;
    //проверка временной метки
    _CheckSequenceCounter(self);
    _CheckTimestampCounter(self);
    if (lastExMode != self->header.headerExtensionMode) {
        self->flags |= RTP_EVENT_EXTHEAD_CHANGE;
    }

    self->lastDataSize = size - headerSize;

    if (self->firstRunStatus) {
        self->firstRunStatus = FALSE;
    }

    return paddingLen;
}

//-------------------------------------------------------------------------------------------------
RtpUnpackerResult RtpUnpacker_Process(RtpUnpacker* self, const uint8_t* datagram, int datagramSize)
{
    RtpUnpackerResult state = {0};
    state.dataOffset = -1;

    //анализ заголовка
    const int paddingSize = _CheckHeader(self, datagram, datagramSize);

    if (paddingSize < 0) {
        state.flags = self->flags;
        return state;
    }

    const int headerSize = calcHeaderSize(&self->header);

    //инициализация структуры состояния
    state.flags = self->flags;
    state.header = self->header;
    state.timestampSkip = self->timestampSkip;
    state.packetsSkipped = self->sequenceSkip;
    state.dataSize = datagramSize - headerSize - paddingSize;
    state.dataOffset = headerSize;
    return state;
}

//-------------------------------------------------------------------------------------------------
void RtpUnpacker_Reset(RtpUnpacker* self)
{
    RtpUnpacker_Init(self);
}
