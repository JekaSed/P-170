#include "slow-shower.h"
#include <QHideEvent>
#include <QPropertyAnimation>
#include <qgraphicseffect.h>

SlowShower::SlowShower(QWidget* parent)
  : QWidget(parent)
  , m_showAnim(new QPropertyAnimation(this))
{
    auto* opacityEffect = new QGraphicsOpacityEffect(this);
    m_showAnim->setTargetObject(opacityEffect);
    m_showAnim->setPropertyName("opacity");
    m_showAnim->setDuration(500);
    setGraphicsEffect(opacityEffect);

    connect(m_showAnim, &QPropertyAnimation::finished, this, [this] {
        QWidget::setVisible(m_needShow);
    });

    m_showAnim->setStartValue(0.0);
    m_showAnim->setEndValue(1.0);
}

void SlowShower::setVisible(bool visible)
{
    if (m_needShow == visible) {
        return;
    }
    m_needShow = visible;
    if (visible) {
        m_showAnim->setDirection(QPropertyAnimation::Forward);
        QWidget::setVisible(true);
    } else {
        m_showAnim->setDirection(QPropertyAnimation::Backward);
    }
    m_showAnim->start();
}
