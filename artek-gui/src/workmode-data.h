#ifndef WORKMODEDATA_H
#define WORKMODEDATA_H

#include <QObject>
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

class WorkModeData : public QObject
{
    Q_OBJECT
public:
    explicit WorkModeData(QObject* parent = nullptr);

private:
    EmissionType m_emission;
    QList<int> m_deviationList;
    int m_selectedDeviation;
    QList<int> m_bitrateList;
    int m_selectedBitrate;

signals:
};

#endif   // WORKMODEDATA_H
