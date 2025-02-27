#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void toBigEndian16(uint8_t* out, uint16_t value);

void toBigEndian32(uint8_t* out, uint32_t value);

uint16_t fromBigEndian16(const uint8_t* value);

uint32_t fromBigEndian32(const uint8_t* value);

#ifdef __cplusplus
}
#endif