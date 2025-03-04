#include "volume-widget.h"
#include "qtmaterialslider.h"
#include "src/frequency/LCDNumber.h"

#include <QJsonObject>

#include <src/menu-button/menu-button.h>

namespace {
const QString title{"Громкость"};

QAbstractButton* makeIconButton(const QIcon& icon, const QString& toolTip, const QSize size)
{
    auto* b = new MenuButton(icon);
    b->setCursor(Qt::PointingHandCursor);
    b->setIconSize(size);
    b->setToolTip(toolTip);
    b->setCheckable(true);
    return b;
}

namespace jsonKey {
const QString volume{"volume"};
const QString isMute{"isMute"};
const QString speaker{"speaker"};
}   // namespace jsonKey

}   // namespace

VolumeWidget::VolumeWidget(QWidget* parent)
  : Cloud{title, parent, theme::foregroundColor_1()}
  , m_slider(new QtMaterialSlider(this))
  , m_valueWidget(new LCDNumber(this))
  , m_muteBt(makeIconButton(theme::muteIcon(), "Выкл", {30, 30}))
{
    //    m_slider->setOrientation(Qt::Vertical);
    m_slider->setMaximum(100);
    //    m_slider->setInvertedAppearance(true);

    auto* mainLy = new QHBoxLayout();

    m_valueWidget->setRange(0, 100);
    m_valueWidget->setInteractive(true);
    m_valueWidget->setSymbolCount(3);
    m_valueWidget->setHeightHint(40);
    m_valueWidget->setNumber(0);
    m_valueWidget->setOnColor(theme::textColor());

    mainLy->addWidget(m_slider, 1, Qt::AlignVCenter);
    mainLy->addWidget(m_muteBt, 0, Qt::AlignVCenter | Qt::AlignRight);
    mainLy->addWidget(m_valueWidget, 0, Qt::AlignVCenter | Qt::AlignRight);
    mainLy->setSpacing(15);
    contentLayout()->addLayout(mainLy);

    connect(m_slider, &QAbstractSlider::valueChanged, this, &VolumeWidget::setVolume);
    connect(m_valueWidget, &LCDNumber::numberChanged, this, &VolumeWidget::setVolume);
    connect(m_muteBt, &QAbstractButton::toggled, this, &VolumeWidget::emitChangedSignal);

    connect(this, &VolumeWidget::volumeChanged, this, [](auto v) {
        qDebug() << "[VolumeWidget]: VOLUME CHANGED" << v;
    });
}

void VolumeWidget::setVolume(const int volume)
{
    if (m_slider->value() == volume && m_valueWidget->currentNumber() == volume) {
        return;
    }
    QSignalBlocker lockSlider(m_slider);
    QSignalBlocker lockNumber(m_valueWidget);
    m_valueWidget->setNumber(volume);   //will fix if out of range
    m_slider->setValue(m_valueWidget->currentNumber());
    emitChangedSignal();
}

QJsonObject VolumeWidget::toJsonObj() const
{
    QJsonObject volJson;
    volJson.insert(jsonKey::isMute, m_muteBt->isChecked());
    volJson.insert(jsonKey::volume, m_valueWidget->currentNumber());
    return {{jsonKey::speaker, volJson}};
}

void VolumeWidget::fromJsonObj(const QJsonObject& obj)
{
    QSignalBlocker lock(this);
    QJsonObject volObj = obj[jsonKey::speaker].toObject();

    setVolume(volObj[jsonKey::volume].toInt());
    m_muteBt->setChecked(volObj[jsonKey::isMute].toBool());
}

void VolumeWidget::emitChangedSignal()
{
    emit volumeChanged(toJsonObj());
}
