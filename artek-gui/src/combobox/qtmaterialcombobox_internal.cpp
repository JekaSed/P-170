#include "qtmaterialcombobox_internal.h"
#include <QPropertyAnimation>
#include <qcoreevent.h>
//#include <qsignaltransition.h>
#include <qwidget.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <QtStateMachine/QStateMachine>
#include <QtStateMachine/QEventTransition>
#include <QtStateMachine/QSignalTransition>
#else
#include <QState>
#include <QAbstractTransition>
#include <QSignalTransition>
#include <QEventTransition>
#endif

#include <QPainter>
#include "qtmaterialcombobox.h"
#include "src/menu/contextmenu.h"

QtMaterialComboBoxStateMachine::QtMaterialComboBoxStateMachine(ContextMenu* menu, QtMaterialComboBox* parent)
  : QStateMachine(parent)
  , m_cb(parent)
  , m_normalState(new QState)
  , m_focusedState(new QState)
  , m_label(nullptr)
  , m_offsetAnimation(0)
  , m_colorAnimation(0)
  , m_progress(0.0)
{
    Q_ASSERT(parent);

    addState(m_normalState);
    addState(m_focusedState);

    setInitialState(m_normalState);

    QPropertyAnimation* animation;
    auto showTrans = new QSignalTransition(menu, &ContextMenu::aboutToShow);
    showTrans->setTargetState(m_focusedState);
    m_normalState->addTransition(showTrans);

    animation = new QPropertyAnimation(this, "progress", this);
    animation->setEasingCurve(QEasingCurve::InCubic);
    animation->setDuration(310);
    showTrans->addAnimation(animation);

    showTrans = new QSignalTransition(menu, &ContextMenu::hided);
    m_focusedState->addTransition(showTrans);
    showTrans->setTargetState(m_normalState);

    animation = new QPropertyAnimation(this, "progress", this);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->setDuration(310);
    showTrans->addAnimation(animation);

    m_normalState->assignProperty(this, "progress", 0);
    m_focusedState->assignProperty(this, "progress", 1);

    setupProperties();
}

QtMaterialComboBoxStateMachine::~QtMaterialComboBoxStateMachine()
{}

void QtMaterialComboBoxStateMachine::setLabel(QtMaterialComboBoxLabel* label)
{
    delete m_label;

    if (m_offsetAnimation != nullptr) {
        removeDefaultAnimation(m_offsetAnimation);
        delete m_offsetAnimation;
    }

    if (m_colorAnimation != nullptr) {
        removeDefaultAnimation(m_colorAnimation);
        delete m_colorAnimation;
    }

    m_label = label;

    if (m_label != nullptr) {
        m_offsetAnimation = new QPropertyAnimation(m_label, "offset", this);
        m_offsetAnimation->setDuration(210);
        m_offsetAnimation->setEasingCurve(QEasingCurve::OutCubic);
        addDefaultAnimation(m_offsetAnimation);

        m_colorAnimation = new QPropertyAnimation(m_label, "color", this);
        m_colorAnimation->setDuration(210);
        addDefaultAnimation(m_colorAnimation);
    }

    setupProperties();
}

bool QtMaterialComboBoxStateMachine::isEmpty()
{
    return m_cb->currentText().isEmpty();
}

void QtMaterialComboBoxStateMachine::setupProperties()
{
    if (m_label != nullptr) {
        const int m = 0;   //m_textField->textMargins().top();

        if (isEmpty()) {
            m_normalState->assignProperty(m_label, "offset", QPointF(0, 26));
        } else {
            m_normalState->assignProperty(m_label, "offset", QPointF(0, 0 - m));
        }

        m_focusedState->assignProperty(m_label, "offset", QPointF(0, 0 - m));
        m_focusedState->assignProperty(m_label, "color", m_cb->inkColor());
        m_normalState->assignProperty(m_label, "color", m_cb->labelColor());

        if (0 != m_label->offset().y() && !isEmpty()) {
            m_label->setOffset(QPointF(0, 0 - m));
        } else if (!m_cb->hasFocus() && m_label->offset().y() <= 0 && isEmpty()) {
            m_label->setOffset(QPointF(0, 26));
        }
    }

    m_cb->update();
}

QtMaterialComboBoxLabel::QtMaterialComboBoxLabel(QtMaterialComboBox* parent)
  : QWidget(parent)
  , m_cb(parent)
  , m_scale(1)
  , m_posX(0)
  , m_posY(26)
  , m_color(parent->labelColor())
{
    Q_ASSERT(parent);

    QFont font("Roboto", parent->labelFontSize(), QFont::Medium);
    font.setLetterSpacing(QFont::PercentageSpacing, 102);
    setFont(font);
}

/*!
 *  \reimp
 */
void QtMaterialComboBoxLabel::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    if (m_text.isEmpty()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.scale(m_scale, m_scale);
    painter.setPen(m_color);
    painter.setOpacity(1);

    QPointF pos(2 + m_posX, m_cb->rect().y() + heightLabel());
    painter.drawText(pos, m_text);
}

int QtMaterialComboBoxLabel::heightLabel() const
{
    return fontMetrics().height();
}
