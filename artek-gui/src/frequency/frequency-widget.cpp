#include "frequency-widget.h"

namespace {
const QString title{"Частота"};
constexpr double minFreq{8000};
constexpr double maxFreq{108000000};
}   // namespace

FrequencyWidget::FrequencyWidget(QWidget* parent)
  : Cloud{title, parent}
  , m_freqEditWidget(new FreqEditWidget(this))
{
    setMinimumSize(300, 150);
    m_freqEditWidget->setMaxFreq(maxFreq);
    m_freqEditWidget->setMinFreq(minFreq);
    m_freqEditWidget->setFreq(4500000);
    m_freqEditWidget->setFreqColor(theme::textColor());
    m_freqEditWidget->setMinimumHeight(55);
    contentLayout()->addWidget(m_freqEditWidget);

    connect(m_freqEditWidget, &FreqEditWidget::freqChanged, this, &FrequencyWidget::freqChanged);
}
