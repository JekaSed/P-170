#include <QList>
#include <QMap>

#pragma once

enum class EmissionType
{
    F1B,
    F7B,
    G1B,
    J3E,
    B8E,
    R3E,
    H3E,
    F3EJ,
    F3EA,
    A1A,
    A3E,
    A2A
};

struct WorkModeDefault
{
    EmissionType m_emission;
    QList<int> m_deviationList;
    QList<int> m_bitrateList;
};

static const QMap<QString, QString> sideBandMap{{"ВБП", "Upper"}, {"НБП", "Lower"}};

WorkModeDefault getDefault(EmissionType emissionClass);
QString emissionToString(const EmissionType emission);
EmissionType stringToEmission(const QString& str);
QStringList emissionTextList();
