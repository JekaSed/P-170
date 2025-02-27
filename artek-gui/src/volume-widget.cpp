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

    connect(m_slider, &QAbstractSlider::valueChanged, this, [this](const int v) {
        qDebug() << "slider" << v;
        setVolume(v);
    });

    connect(m_valueWidget, &LCDNumber::numberChanged, this, [this](int v) {
        qDebug() << "LCDNumber" << v;
        setVolume(v);
    });

    connect(m_muteBt, &QAbstractButton::toggled, this, [this](bool checked) {
        qDebug() << "muted" << m_muteBt->isChecked();
        if (checked) {
            m_lastMutedValue = getVolume();
            setVolume(0);
        } else {
            //if clicked
            if (m_slider->value() == 0 || m_valueWidget->currentNumber() == 0) {
                setVolume(m_lastMutedValue > 0 ? m_lastMutedValue : 1);
            }
        }
    });

    connect(this, &VolumeWidget::volumeChanged, this, [](auto v) {
        qDebug() << "VOLUME CHANGED" << v;
    });
}

void VolumeWidget::setVolume(const int volume)
{
    if (m_slider->value() == volume && m_valueWidget->currentNumber() == volume &&
        m_muteBt->isChecked() == (volume == 0)) {
        return;
    }
    QSignalBlocker lockSlider(m_slider);
    QSignalBlocker lockNumbder(m_valueWidget);
    //    QSignalBlocker lockMuteBt(m_muteBt); //QMaterial not updating when blocked
    m_valueWidget->setNumber(volume);   //will fix if out of range
    const int changedValue = getVolume();
    m_slider->setValue(changedValue);
    m_muteBt->setChecked(changedValue == 0);

    emit volumeChanged(toJsonObj());
}

int VolumeWidget::getVolume() const
{
    return m_valueWidget->currentNumber();
}

QJsonObject VolumeWidget::toJsonObj() const
{
    QJsonObject volJson;
    bool isMute = m_muteBt->isChecked();
    volJson.insert(jsonKey::isMute, isMute);
    volJson.insert(jsonKey::volume, isMute ? m_lastMutedValue : getVolume());
    return {{jsonKey::speaker, volJson}};
}

void VolumeWidget::fromJsonObj(const QJsonObject& obj)
{
    QJsonObject volObj = obj[jsonKey::speaker].toObject();

    const bool isMute = volObj[jsonKey::isMute].toBool();
    const int volume = volObj[jsonKey::volume].toInt();

    QSignalBlocker lock(this);
    if (isMute) {
        setVolume(volume);
        m_muteBt->setChecked(true);
    } else {
        m_muteBt->setChecked(false);
        setVolume(volume);
    }
}
