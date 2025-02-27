#include "artek-widget.h"
#include "delayed.h"
#include "qboxlayout.h"

#include <QJsonObject>
#include <QLabel>

namespace {
namespace jsonKey {
const QString demodulator{"demodulator"};
const QString deviceState{"deviceState"};
const QString tlfOutputs{"tlfOutputs"};

const QString hfChannelParam{"hfChannelParam"};

}   // namespace jsonKey
}   // namespace

ArtekWidget::ArtekWidget(QWidget* parent)
  : QWidget{parent}
  , m_freqWidget(new FrequencyWidget(this))
  , m_volWidget(new VolumeWidget(this))
  , m_wmWidget(new WorkModeWidget(this))
{
    //    setObjectName("ArtekWidget");
    //    setStyleSheet(QString("QWidget#ArtekWidget {background-color: %1}").arg(theme::foregroundColor_2().name()));
    //    setAttribute(Qt::WA_Hover);
    setAttribute(Qt::WA_StyledBackground);

    auto mainLy = new QVBoxLayout(this);
    mainLy->addWidget(m_freqWidget);
    mainLy->addWidget(m_volWidget);
    mainLy->addWidget(m_wmWidget);

    m_lastSendTimer.setInterval(500);
    m_lastSendTimer.setSingleShot(true);
    connect(&m_lastSendTimer, &QTimer::timeout, this, &ArtekWidget::sendAll);

    connect(m_freqWidget, &FrequencyWidget::freqChanged, this, [this](const QJsonObject& f) {
        sendChange(jsonKey::hfChannelParam, f);
    });
    connect(m_volWidget, &VolumeWidget::volumeChanged, this, [this](const QJsonObject& wm) {
        sendChange(jsonKey::tlfOutputs, wm);
    });
    connect(m_wmWidget, &WorkModeWidget::changed, this, [this](const QJsonObject& volume) {
        sendChange(jsonKey::demodulator, volume);
    });
}

QJsonObject ArtekWidget::toJsonObj() const
{
    QJsonObject result;
    result.insert(jsonKey::demodulator, m_wmWidget->toJsonObj());
    result.insert(jsonKey::hfChannelParam, m_freqWidget->toJsonObj());
    result.insert(jsonKey::tlfOutputs, m_volWidget->toJsonObj());
    return result;
}

void ArtekWidget::fromJsonObj(const QJsonObject& obj)
{
    m_wmWidget->fromJsonObj(obj[jsonKey::demodulator].toObject());
    m_freqWidget->fromJsonObj(obj[jsonKey::hfChannelParam].toObject());
    m_volWidget->fromJsonObj(obj[jsonKey::tlfOutputs].toObject());
}

bool ArtekWidget::isValidJson(const QJsonObject& obj)
{
    auto keys = obj.keys();
    if (keys.contains(jsonKey::demodulator) && keys.contains(jsonKey::hfChannelParam) &&
        keys.contains(jsonKey::tlfOutputs)) {
        return true;
    }
    return false;
}

void ArtekWidget::emitChangedSignal(const QString& key, const QJsonObject& value)
{
    emit changed({{key, value}});
    m_lastSendTimer.start();
}

void ArtekWidget::sendChange(const QString& key, const QJsonObject& value)
{
    if (m_lastSendTimer.isActive()) {
        m_changes.insert(key, value);
    } else {
        emitChangedSignal(key, value);
    }
}

void ArtekWidget::sendAll()
{
    for (const auto& key : m_changes.keys()) {
        emitChangedSignal(key, m_changes.take(key));
    }
}

void ArtekWidget::setNewState(const QJsonObject& newState)
{
    fromJsonObj(newState);
}
