// #ifndef RTPCOMMON_H
// #define RTPCOMMON_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define RTP_DEFAULT_HEADER_SIZE (12)   ///< размер RTP заголовка по-умолчанию
#define RTP_MAX_CSRC_SIZE (15)         ///< максимальный размер RTP заголовка
#define RTP_MAX_EXTENSION_SIZE (15)    ///< максимальный размер RTP заголовка
#define RTP_MAX_DROP_SEQ (2)   ///< макс. кол-во потерянных пакетов чтобы обновить синхронизацию

#ifdef _WIN32
#define DECL_EXPORT __declspec(dllexport)
#define DECL_IMPORT __declspec(dllimport)
#elif defined(__linux__)
#define DECL_EXPORT __attribute__((visibility("default")))
#define DECL_IMPORT __attribute__((visibility("default")))
#endif

#include <stdint.h>

enum RtpUnpackerEvent
{
    RTP_EVENT_FORMAT_ERR = 1,              ///< ошибка формата
    RTP_EVENT_SEQUENCE_NUM_ERR = 1 << 1,   ///< ошибка нумерации пакетов
    RTP_EVENT_TIMESTAMP_ERR = 1 << 2,      ///< ошибка временной метки
    RTP_EVENT_SRCCH_CHANGE = 1 << 3,       ///< канал источника был изменен
    RTP_EVENT_EXTHEAD_CHANGE = 1 << 4,   ///< изменилось состояние расширенного заголовка
    RTP_EVENT_ORDER_ERR = 1 << 5,   ///< нарушен порядок пакета (например 2, 3, 5, 4...)
    RTP_EVENT_UNSYNC_ERR = 1 << 6   ///< потеря синхронизации по номеру пакета
};

typedef struct RtpHeader
{
    uint8_t cc;   ///< содержит количество идентификаторов CSRC
    uint8_t marker;
    uint8_t packetType;                  ///< тип пакета
    uint8_t headerExtensionMode;         ///< статус расширенного заголовка
    uint16_t extensionSize;              ///< размер расширенного заголовка
    uint16_t sequenceNum;                ///< счетчик пакетов
    uint16_t extensionProfileData;       ///< профайл расширенного заголовка
    uint8_t padding;                     ///< отсуп
    uint32_t timeStamp;                  ///< временной счетчик
    uint32_t ssrc;                       ///< идентификатор канала
    uint32_t csrcs[RTP_MAX_CSRC_SIZE];   ///< CSRC-список идентифицирует источники
                                         ///< информации
    uint32_t extensionData[RTP_MAX_EXTENSION_SIZE];   ///< дополнительные данные при
                                                      ///< расширении заголовка
} RtpHeader;

int calcHeaderSize(RtpHeader* self);

#ifdef __cplusplus
}
#endif

// #endif // RTPCOMMON_H
