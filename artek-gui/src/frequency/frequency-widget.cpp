#include "frequency-widget.h"

#include <QJsonObject>

namespace {
const QString title{"Частота"};
constexpr double minFreq{8000};
constexpr double maxFreq{108000000};

namespace jsonKey {
const QString freq{"freq"};
}

}   // namespace

FrequencyWidget::FrequencyWidget(QWidget* parent)
  : Cloud{title, parent, theme::foregroundColor_1()}
  , m_freqEditWidget(new FreqEditWidget(this))
{
    setMinimumSize(300, 150);
    m_freqEditWidget->setMaxFreq(maxFreq);
    m_freqEditWidget->setMinFreq(minFreq);
    m_freqEditWidget->setFreq(4500000);
    m_freqEditWidget->setFreqColor(theme::textColor());
    m_freqEditWidget->setMinimumHeight(55);
    contentLayout()->addWidget(m_freqEditWidget);

    connect(m_freqEditWidget, &FreqEditWidget::freqChanged, this, [this]() {
        emit freqChanged(toJsonObj());
    });
}

QJsonObject FrequencyWidget::toJsonObj() const
{
    return {{jsonKey::freq, int(m_freqEditWidget->freq())}};
}

void FrequencyWidget::fromJsonObj(const QJsonObject& obj)
{
    m_freqEditWidget->setFreq(obj[jsonKey::freq].toInteger());
}
