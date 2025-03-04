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
const QString workMode{"workMode"};
const QString sideBand{"sideBand"};

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
    m_selectedDeviation = m_deviationList.isEmpty() ? -1 : m_deviationList.first();
    bitrateDefault();
    if (m_emission == EmissionType::J3E || m_emission == EmissionType::R3E || m_emission == EmissionType::H3E) {
        m_sideBand = 0;
    } else {
        m_sideBand = -1;
    }
    lock.unblock();
    emit updateView();
}

EmissionType WorkModeData::emission() const
{
    return m_emission;
}

void WorkModeData::loadDefault(EmissionType newEmission)
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
    QJsonObject workModeObj;
    workModeObj.insert(jsonKey::emission, emissionToString(m_emission));
    if (m_selectedDeviation > 0) {
        workModeObj.insert(jsonKey::deviation, m_selectedDeviation);
    }
    if (m_selectedBitrate > 0) {
        workModeObj.insert(jsonKey::bitrate, m_selectedBitrate);
    }
    if (m_sideBand == 0 || m_sideBand == 1) {
        workModeObj.insert(jsonKey::sideBand, sideBandMap.values()[m_sideBand]);
    }
    return {{jsonKey::workMode, workModeObj}};
}

void WorkModeData::fromJsonObj(const QJsonObject& obj)
{
    QJsonObject workModeJson = obj[jsonKey::workMode].toObject();
    m_emission = stringToEmission(workModeJson[jsonKey::emission].toString());
    auto defWM = getDefault(m_emission);
    m_deviationList = defWM.m_deviationList;
    m_bitrateList = defWM.m_bitrateList;
    m_selectedDeviation = workModeJson[jsonKey::deviation].toInt(-1);
    m_selectedBitrate = workModeJson[jsonKey::bitrate].toInt(-1);

    emit updateView();
}

int WorkModeData::sideBand() const
{
    return m_sideBand;
}

void WorkModeData::setSideBand(int newSideBand)
{
    if (m_sideBand == newSideBand)
        return;
    m_sideBand = newSideBand;
    emit sideBandChanged(newSideBand);
}

bool WorkModeData::isValid() const
{
    switch (m_emission) {
    case EmissionType::F1B:
    case EmissionType::F7B:
        return m_selectedDeviation > 0 && m_selectedBitrate > 0;
    case EmissionType::G1B:
        return m_selectedBitrate > 0;
    case EmissionType::J3E:
    case EmissionType::R3E:
    case EmissionType::H3E:
        return m_sideBand == 0 || m_sideBand == 1;
    case EmissionType::A1A:
        return m_selectedDeviation > 0;
    case EmissionType::B8E:
    case EmissionType::F3EJ:
    case EmissionType::F3EA:
    case EmissionType::A3E:
    case EmissionType::A2A:
        break;
    }
    return true;
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
