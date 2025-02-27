#pragma once

namespace network {

enum class WiretapFormat
{
    // Сигнал с тюнера (24-битные комплексные отчеты).
    IntermediateFreq,
    // Сигнал после демодуляции (16-битные реальные).
    LowerFreq

};

}
