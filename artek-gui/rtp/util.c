#include "util.h"

void toBigEndian16(uint8_t* out, uint16_t value)
{
    out[0] = (value >> 8) & 0xFF;
    out[1] = value & 0xFF;
}

void toBigEndian32(uint8_t* out, uint32_t value)
{
    out[0] = (value >> 24) & 0xFF;
    out[1] = (value >> 16) & 0xFF;
    out[2] = (value >> 8) & 0xFF;
    out[3] = value & 0xFF;
}

uint16_t fromBigEndian16(const uint8_t* value)
{
    uint16_t out = value[1];
    out |= value[0] << 8;
    return out;
}

uint32_t fromBigEndian32(const uint8_t* value)
{
    uint32_t out = value[3];
    out |= value[2] << 8;
    out |= value[1] << 16;
    out |= value[0] << 24;
    return out;
}
