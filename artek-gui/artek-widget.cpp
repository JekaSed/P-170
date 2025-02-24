#include "artek-widget.h"
#include "qboxlayout.h"

#include <QLabel>

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
