#include "artek-widget.h"
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

    connect(m_freqWidget, &FrequencyWidget::freqChanged, this, [this](const QJsonObject& f) {
        emit changed({{jsonKey::hfChannelParam, f}});
    });
    connect(m_volWidget, &VolumeWidget::volumeChanged, this, [this](const QJsonObject& wm) {
        emit changed({{jsonKey::tlfOutputs, wm}});
    });
    connect(m_wmWidget, &WorkModeWidget::changed, this, [this](const QJsonObject& volume) {
        emit changed({{jsonKey::demodulator, volume}});
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
