#pragma once

#include "src/workmode-data.h"

struct WorkModeDefault
{
    EmissionType m_emission;
    QList<int> m_deviationList;
    QList<int> m_bitrateList;
};

WorkModeDefault getDefault(EmissionType emissionClass);
