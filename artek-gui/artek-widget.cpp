#include "artek-widget.h"
#include "qboxlayout.h"

#include <QJsonObject>
#include <QLabel>

namespace {
namespace jsonKey {
const QString demodulator{"demodulator"};
const QString workMode{"workMode"};
const QString deviceState{"deviceState"};
const QString tlfOutputs{"tlfOutputs"};
const QString speaker{"speaker"};
const QString hfChannelParam{"hfChannelParam"};

}   // namespace jsonKey
}   // namespace

ArtekWidget::ArtekWidget(QWidget* parent)
  : QWidget{parent}
  , m_freqWidget(new FrequencyWidget(this))
  , m_volWidget(new VolumeWidget(this))
  , m_wmWidget(new WorkModeWidget(this))
{
    auto mainLy = new QHBoxLayout(this);
    mainLy->addWidget(m_freqWidget);
    mainLy->addWidget(m_volWidget);
    mainLy->addWidget(m_wmWidget);
}

QJsonObject ArtekWidget::toJsonObj() const
{
    QJsonObject result;
    result.insert(jsonKey::demodulator, QJsonObject{{jsonKey::workMode, m_wmWidget->toJsonObj()}});
    result.insert(jsonKey::hfChannelParam, m_freqWidget->toJsonObj());
    result.insert(jsonKey::tlfOutputs, QJsonObject{{jsonKey::speaker, m_volWidget->toJsonObj()}});
    return result;
}

void ArtekWidget::fromJsonObj(const QJsonObject& obj)
{
    m_wmWidget->fromJsonObj(obj[jsonKey::demodulator].toObject()[jsonKey::workMode].toObject());
    m_freqWidget->fromJsonObj(obj[jsonKey::hfChannelParam].toObject());
    m_volWidget->fromJsonObj(obj[jsonKey::tlfOutputs].toObject()[jsonKey::speaker].toObject());
}
