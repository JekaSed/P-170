#ifndef WORKMODEDATA_H
#define WORKMODEDATA_H

#include "src/workmode-default.h"
#include <QObject>

class WorkModeData : public QObject
{
    Q_OBJECT
public:
    explicit WorkModeData(QObject* parent = nullptr);
    void fromWorkModeDefault(const WorkModeDefault&);

    EmissionType emission() const;
    void loadDefault(EmissionType newEmission);

    QList<int> deviationList() const;
    int deviation() const;
    void setDeviation(int newSelectedDeviation);

    QList<int> bitrateListAvailable() const;

    int bitrate() const;
    void setBitrate(int newSelectedBitrate);

    //JSON
    QJsonObject toJsonObj() const;
    void fromJsonObj(const QJsonObject& obj);

    int sideBand() const;
    void setSideBand(int newSideBand);

    bool isValid() const;

private:
    EmissionType m_emission{EmissionType::F1B};
    QList<int> m_deviationList{};
    int m_selectedDeviation{-1};
    QList<int> m_bitrateList{};
    int m_selectedBitrate{-1};
    int m_sideBand{0};

private:
    void bitrateDefault();

signals:
    void updateView();
    void selectedDeviationChanged(int value);
    void selectedBitrateChanged(int value);
    void sideBandChanged(int index);
};

#endif   // WORKMODEDATA_H
