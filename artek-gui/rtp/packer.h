#pragma once

#include "common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RtpPacker RtpPacker;

/*!
 * \brief Создание объекта запаковщика
 * \return Хэндл устройства
 */
DECL_EXPORT RtpPacker* RtpPacker_Create();

/*!
 * \brief Удаление объекта запаковщика
 * \param self Хэндл устройства
 */
DECL_EXPORT void RtpPacker_Destroy(RtpPacker* self);

/*!
 * \brief Статическая инициализация устройства
 * \param self Хэндл устройства
 */
DECL_EXPORT void RtpPacker_Init(RtpPacker* self);

/*!
 * \brief Процессинг (запаковка в датаграмму)
 * \param self Хэндл устройства
 * \param[in] data Указатель на блок данных
 * \param[in] size Размер блока данных (должен быть четной величиной!)
 * \param[out] datagramm Указатель на блок данных для формирования датаграммы с
 * заголовоком \param[in] maxSize Размер блока под датаграмму (size + 36)
 * \return Размер датаграммы после запаковки
 */
DECL_EXPORT int RtpPacker_Process(RtpPacker* self, const uint8_t* data, int size, uint8_t* datagramm, int maxSize);

/*!
 * \brief Включение расширенного заголовка RTP пакета
 * \param self Хэндл устройства
 * \param enabled Статус заголовка
 */
DECL_EXPORT void RtpPacker_SetExtensionHeader(RtpPacker* self, int enabled);

/*!
 * \brief Получить состояние расширенного заголовка
 * \param self Хэндл устройства
 * \param[out] size Размер расширенного заголовка в байтах
 * \return TRUE(1) - вкл, FALSE(0) - выкл
 */
DECL_EXPORT int RtpPacker_GetExtensionHeader(RtpPacker* self, int* size);

/*!
 * \brief Задать значение поля заголовка SSRC
 * \param self Хэндл устройства
 * \param ssrc Значение поля (32-бита)
 */
DECL_EXPORT void RtpPacker_SetSsrc(RtpPacker* self, uint32_t ssrc);

/*!
 * \brief Получить текущее значение поля заголовка
 * \param self Хэндл устройства
 * \return Значение поля (32-бита)
 */
DECL_EXPORT unsigned int RtpPacker_GetSsrc(RtpPacker* self);

/** Установить CSRC.*/
DECL_EXPORT void RtpPacker_SetCsrc(RtpPacker* self, int count, const uint32_t* csrcs);

/** Получить CSRC.*/
DECL_EXPORT uint32_t* RtpPacker_GetCsrc(RtpPacker* self);

/** Получить количество CSRC.*/
DECL_EXPORT int RtpPacker_GetCsrcCount(RtpPacker* self);

/** Задать расширенный загаловок.*/
DECL_EXPORT void RtpPacker_SetExtensionHeaderData(RtpPacker* self, int count, const uint32_t* header);

/** Установить тип пакета.*/
DECL_EXPORT void RtpPacker_SetPacketType(RtpPacker* self, uint8_t pt);

/** Получить тип пакета.*/
DECL_EXPORT uint8_t RtpPacker_GetPacketType(RtpPacker* self);

/** Установить маркер.*/
DECL_EXPORT void RtpPacker_SetMarker(RtpPacker* self, uint8_t pt);

/** Получить маркер.*/
DECL_EXPORT uint8_t RtpPacker_GetMarker(RtpPacker* self);

/** Получить отступ.*/
DECL_EXPORT void RtpPacker_SetPadding(RtpPacker* self, uint8_t padding);

/** Установить профайл.*/
DECL_EXPORT void RtpPacker_SetProfile(RtpPacker* self, uint16_t pt);

/** Получить профайл.*/
DECL_EXPORT uint16_t RtpPacker_GetProfile(RtpPacker* self);
/*!
 * \brief RtpPacker_GetGrs
 * \brief Сброс состояния запаковщика
 * \param self Хэндл устройства
 */
DECL_EXPORT void RtpPacker_Reset(RtpPacker* self);

#ifdef __cplusplus
}
#endif
