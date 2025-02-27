#pragma once

#include "unpacker.h"
#include <stdint.h>

/*!
 * \brief Параметры распаковщика
 */
struct RtpUnpacker
{
    RtpHeader header;
    uint32_t lastTimestamp;
    uint32_t timestampSkip;   ///< пропущено отсчетов для этого пакета относительно
                              ///< прошлого
    uint32_t sequenceSkip;   ///< пропущено пакетов для этого пакета относительно
                             ///< прошлого
    uint32_t flags;
    int lastDataSize;   /// размер последнего пакета
    uint16_t lastSeq;
    uint8_t firstRunStatus;   ///< статус первого запуска
};
