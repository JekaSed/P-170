#pragma once

#include "common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RtpUnpacker RtpUnpacker;

/*!
 * \brief Структура состояния распаковки
 */
typedef struct RtpUnpackerResult
{
    RtpHeader header;

    uint32_t flags;
    int timestampSkip;
    int packetsSkipped;
    int dataOffset;   // смещение в буфере данных (- 1 в случае ошибки)
    int dataSize;     // размер данных
} RtpUnpackerResult;

/*!
 * \brief Конструктор модуля
 * \return Хэндл устройства
 */
DECL_EXPORT RtpUnpacker* RtpUnpacker_Create();

/*!
 * \brief Деструктор модуля
 * \param self Хэндл устройства
 */
DECL_EXPORT void RtpUnpacker_Destroy(RtpUnpacker* self);

/*!
 * \brief Статическая инициализация устройства
 */
DECL_EXPORT void RtpUnpacker_Init(RtpUnpacker* self);

/*!
 * \brief Процессинг запаковки
 * \param self указатель на модуль
 * \param datagram Указатель на RTP датаграмму
 * \param datagramSize Размер датаграммы
 * \return Статус распаковки
 */
DECL_EXPORT struct RtpUnpackerResult RtpUnpacker_Process(RtpUnpacker* self, const uint8_t* datagram, int datagramSize);

/*!
 * \brief Сброс параметров
 * \param self Хэндл устройства
 */
DECL_EXPORT void RtpUnpacker_Reset(RtpUnpacker* self);

#ifdef __cplusplus
}
#endif
