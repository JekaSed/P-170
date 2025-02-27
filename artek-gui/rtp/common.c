#include "common.h"

int calcHeaderSize(RtpHeader* self)
{
    int size = RTP_DEFAULT_HEADER_SIZE;
    if (self->headerExtensionMode) {
        size += (1 + self->extensionSize) * sizeof(uint32_t);
    }
    size += self->cc * sizeof(uint32_t);
    return size;
}
