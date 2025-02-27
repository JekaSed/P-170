#include "common.h"
#include "packer.h"
#include "packer_t.h"
#include "util.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

//-------------------------------------------------------------------------------------------------
RtpPacker* RtpPacker_Create()
{
    RtpPacker* self = (RtpPacker*)malloc(sizeof(RtpPacker));
    RtpPacker_Init(self);
    return self;
}

//-------------------------------------------------------------------------------------------------
void RtpPacker_Destroy(RtpPacker* self)
{
    free(self);
}

//-------------------------------------------------------------------------------------------------
void RtpPacker_Init(RtpPacker* self)
{
    memset(&self->header, 0, sizeof(RtpHeader));
    self->header.ssrc = (uint32_t)rand() * rand();
}

//-------------------------------------------------------------------------------------------------
static void _UpdateCounter(RtpHeader* self, int size)
{
    self->sequenceNum = (((uint16_t)self->sequenceNum) + 1) & 0xFFFF;
    self->timeStamp = ((uint32_t)self->timeStamp + (size)) & 0xFFFFFFFF;
}

//-------------------------------------------------------------------------------------------------
static void _FormHeader(RtpHeader* self, uint8_t* datagram)
{
    //conts header (big endian)
    datagram[0] = 0b10000000;
    datagram[0] |= self->cc & 0b1111;
    if (self->headerExtensionMode) {
        datagram[0] |= 1 << 4;
    }
    datagram[1] = self->marker << 7;
    datagram[1] |= self->packetType & ((1 << 7) - 1);

    //sequence number (big endian)
    toBigEndian16(datagram + 2, self->sequenceNum);

    //timestamp (big endian)
    toBigEndian32(datagram + 4, self->timeStamp);

    //ssrc (big endian)
    toBigEndian32(datagram + 8, self->ssrc);

    int offset = 12;
    if (self->cc > 0) {
        for (int i = 0; i < self->cc; ++i) {
            toBigEndian32(datagram + offset + i * 4, self->csrcs[i]);
        }
        offset += self->cc * 4;
    }
    //header extension
    if (self->headerExtensionMode) {
        toBigEndian16(datagram + offset, self->extensionProfileData);
        toBigEndian16(datagram + offset + 2, self->extensionSize);
        offset += 4;
        for (int i = 0; i < self->extensionSize; ++i) {
            toBigEndian32(datagram + offset + i * 4, self->extensionData[i]);
        }
    }
}

static int _PaddingSize(RtpHeader* self, int size)
{
    if (self->padding && size % sizeof(uint32_t) != 0) {
        return (sizeof(uint32_t) - size % sizeof(uint32_t)) % sizeof(uint32_t);
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------
int RtpPacker_Process(RtpPacker* self, const uint8_t* data, int size, uint8_t* datagram, int maxSize)
{
    if ((!self->header.padding && size % 2 != 0) || size == 0) {
        return -1;
    }

    const int headerSize = calcHeaderSize(&self->header);
    const int paddingSize = _PaddingSize(&self->header, size);

    //проверка размеров буферов
    if (size + headerSize + paddingSize > maxSize) {
        return 0;
    }

    //формирование заголовка
    _FormHeader(&self->header, datagram);

    //формирование блока данных
    memcpy(datagram + headerSize, data, size);

    if (paddingSize > 0) {
        datagram[0] |= 1 << 5;
        datagram[size + headerSize + paddingSize - 1] = paddingSize;
    }

    //обновление счетчиков
    _UpdateCounter(&self->header, size);

    return (size + headerSize + paddingSize);
}

//-------------------------------------------------------------------------------------------------
void RtpPacker_SetExtensionHeader(RtpPacker* self, int enabled)
{
    self->header.headerExtensionMode = enabled & 0b1;
}

//-------------------------------------------------------------------------------------------------
void RtpPacker_SetSsrc(RtpPacker* self, uint32_t ssrc)
{
    self->header.ssrc = ssrc;
}

//-------------------------------------------------------------------------------------------------
unsigned int RtpPacker_GetSsrc(RtpPacker* self)
{
    return self->header.ssrc;
}

//-------------------------------------------------------------------------------------------------
void RtpPacker_Reset(RtpPacker* self)
{
    self->header.sequenceNum = 0;
    self->header.timeStamp = 0;
}

//-------------------------------------------------------------------------------------------------
void RtpPacker_SetCsrc(RtpPacker* self, int count, const uint32_t* csrcs)
{
    assert(count <= RTP_MAX_CSRC_SIZE);
    memcpy(self->header.csrcs, csrcs, count * sizeof(uint32_t));
    self->header.cc = count;
}

//-------------------------------------------------------------------------------------------------
uint32_t* RtpPacker_GetCsrc(RtpPacker* self)
{
    return self->header.csrcs;
}

//-------------------------------------------------------------------------------------------------
int RtpPacker_GetCsrcCount(RtpPacker* self)
{
    return self->header.cc;
}

//-------------------------------------------------------------------------------------------------
void RtpPacker_SetExtensionHeaderData(RtpPacker* self, int count, const uint32_t* data)
{
    assert(count <= RTP_MAX_EXTENSION_SIZE);
    self->header.extensionSize = count;
    memcpy(self->header.extensionData, data, count * sizeof(uint32_t));
}

//-------------------------------------------------------------------------------------------------
void RtpPacker_SetPacketType(RtpPacker* self, uint8_t pt)
{
    self->header.packetType = pt;
}

//-------------------------------------------------------------------------------------------------
uint8_t RtpPacker_GetPacketType(RtpPacker* self)
{
    return self->header.packetType;
}

//-------------------------------------------------------------------------------------------------
void RtpPacker_SetMarker(RtpPacker* self, uint8_t marker)
{
    self->header.marker = marker & 0b1;
}

//-------------------------------------------------------------------------------------------------
uint8_t RtpPacker_GetMarker(RtpPacker* self)
{
    return self->header.marker;
}

//-------------------------------------------------------------------------------------------------
void RtpPacker_SetPadding(RtpPacker* self, uint8_t padding)
{
    self->header.padding = padding & 0b1;
}

//-------------------------------------------------------------------------------------------------
void RtpPacker_SetProfile(RtpPacker* self, uint16_t pt)
{
    self->header.extensionProfileData = pt;
}

//-------------------------------------------------------------------------------------------------
uint16_t RtpPacker_GetProfile(RtpPacker* self)
{
    return self->header.extensionProfileData;
}
