#include "workmode-data.h"
#include "qdebug.h"

#include <QJsonObject>

namespace {
namespace jsonKey {

const QString emission{"emission"};
const QString deviation{"deviation"};
const QString bitrate{"bitrate"};

const QString power{"power"};
const QString diagnostics{"diagnostics"};
const QString mode{"mode"};
const QString state{"state"};
const QString errorCode{"errorCode"};
const QString halfDuplexMode{"halfDuplexMode"};

}   // namespace jsonKey
}   // namespace

WorkModeData::WorkModeData(QObject* parent)
  : QObject{parent}
{}

void WorkModeData::fromWorkModeDefault(const WorkModeDefault& wmDefault)
{
    QSignalBlocker lock(this);
    m_emission = wmDefault.m_emission;
    m_deviationList = wmDefault.m_deviationList;
    m_bitrateList = wmDefault.m_bitrateList;
    m_selectedDeviation = -1;
    bitrateDefault();
    lock.unblock();
    emit updateView();
}

EmissionType WorkModeData::emission() const
{
    return m_emission;
}

void WorkModeData::setEmission(EmissionType newEmission)
{
    fromWorkModeDefault(getDefault(newEmission));
}

QList<int> WorkModeData::deviationList() const
{
    return m_deviationList;
}

int WorkModeData::deviation() const
{
    return m_selectedDeviation;
}

void WorkModeData::setDeviation(int newSelectedDeviation)
{
    if (m_selectedDeviation == newSelectedDeviation)
        return;
    m_selectedDeviation = newSelectedDeviation;
    bitrateDefault();
    emit selectedDeviationChanged(newSelectedDeviation);
}

QList<int> WorkModeData::bitrateListAvailable() const
{
    QList<int> result;
    if (m_emission == EmissionType::F1B && m_selectedDeviation == 6) {
        return {4};
    }

    if (m_emission == EmissionType::G1B) {
        return m_bitrateList;
    }

    for (const auto& b : m_bitrateList) {
        if (b < m_selectedDeviation)
            result.append(b);
    }
    return result;
}

int WorkModeData::bitrate() const
{
    return m_selectedBitrate;
}

void WorkModeData::setBitrate(int newSelectedBitrate)
{
    if (m_selectedBitrate == newSelectedBitrate)
        return;
    m_selectedBitrate = newSelectedBitrate;
    emit selectedBitrateChanged(newSelectedBitrate);
}

QJsonObject WorkModeData::toJsonObj() const
{
    QJsonObject workMode;
    workMode.insert(jsonKey::emission, emissionToString(m_emission));
    if (m_selectedDeviation > 0) {
        workMode.insert(jsonKey::deviation, m_selectedDeviation);
    }
    if (m_selectedBitrate > 0) {
        workMode.insert(jsonKey::bitrate, m_selectedBitrate);
    }
    return workMode;
}

void WorkModeData::fromJsonObj(const QJsonObject& obj)
{
    m_emission = stringToEmission(obj[jsonKey::emission].toString());
    auto defWM = getDefault(m_emission);
    m_deviationList = defWM.m_deviationList;
    m_bitrateList = defWM.m_bitrateList;
    m_selectedDeviation = obj[jsonKey::deviation].toInt(-1);
    m_selectedBitrate = obj[jsonKey::bitrate].toInt(-1);

    emit updateView();
}

void WorkModeData::bitrateDefault()
{
    int bitRate{-1};
    if (m_emission == EmissionType::F1B) {
        switch (m_selectedDeviation) {
        case 6:
            bitRate = 4;
            break;
        case 75:
        case 100:
            bitRate = 50;
            break;
        case 125:
        case 200:
        case 250:
            bitRate = 100;
            break;
        case 500:
            bitRate = 300;
            break;
        case 1000:
            bitRate = 500;
        default:
            bitRate = -1;
            break;
        }
    } else if (m_emission == EmissionType::F7B) {
        bitRate = 50;
    } else if (m_emission == EmissionType::G1B) {
        bitRate = 100;
    } else {
        bitRate = -1;
    }

    m_selectedBitrate = bitRate;
}
