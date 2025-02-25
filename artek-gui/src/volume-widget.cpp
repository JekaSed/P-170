#include "volume-widget.h"
#include "qtmaterialslider.h"
#include "src/frequency/LCDNumber.h"

#include <QJsonObject>
#include <QTimer>

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
}   // namespace jsonKey

}   // namespace

VolumeWidget::VolumeWidget(QWidget* parent)
  : Cloud{title, parent}
  , m_slider(new QtMaterialSlider(this))
  , m_valueWidget(new LCDNumber(this))
  , m_muteBt(makeIconButton(theme::muteIcon(), "Выкл", {30, 30}))
{
    m_slider->setOrientation(Qt::Vertical);
    m_slider->setMaximum(100);
    m_slider->setInvertedAppearance(true);

    m_valueWidget->setRange(0, 100);
    m_valueWidget->setInteractive(true);
    m_valueWidget->setSymbolCount(3);
    m_valueWidget->setHeightHint(40);
    m_valueWidget->setNumber(0);
    m_valueWidget->setOnColor(theme::textColor());

    contentLayout()->addWidget(m_slider, 1, Qt::AlignHCenter);
    contentLayout()->addWidget(m_muteBt, 0, Qt::AlignHCenter | Qt::AlignTop);
    contentLayout()->addWidget(m_valueWidget, 0, Qt::AlignHCenter | Qt::AlignTop);
    contentLayout()->setSpacing(15);

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

    connect(this, &VolumeWidget::volumeChanged, this, [](int v) {
        qDebug() << "VOLUME CHANGED" << v;
    });

    QTimer::singleShot(100, this, [this]() {
        setVolume(44);
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

    emit volumeChanged(changedValue);
}

int VolumeWidget::getVolume() const
{
    return m_valueWidget->currentNumber();
}

QJsonObject VolumeWidget::toJsonObj() const
{
    QJsonObject result;
    bool isMute = m_muteBt->isChecked();
    result.insert(jsonKey::isMute, isMute);
    result.insert(jsonKey::volume, isMute ? m_lastMutedValue : getVolume());
    return result;
}

void VolumeWidget::fromJsonObj(const QJsonObject& obj)
{
    const bool isMute = obj[jsonKey::isMute].toBool();
    const int volume = obj[jsonKey::volume].toInt();

    QSignalBlocker lock(this);
    if (isMute) {
        setVolume(volume);
        m_muteBt->setChecked(true);
    } else {
        m_muteBt->setChecked(false);
        setVolume(volume);
    }
}
