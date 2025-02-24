#pragma once

#include <QtGlobal>
#include <qglobal.h>
#include <qwidget.h>
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <QtStateMachine/QStateMachine>
#include <QtStateMachine/QEventTransition>

#include <src/menu/contextmenu.h>
#else
#include <QStateMachine>
#include <QEventTransition>
#endif

#include <QtWidgets/QWidget>
#include "qtmaterialcombobox.h"

class QPropertyAnimation;
class QtMaterialComboBoxLabel;

class QtMaterialComboBoxStateMachine : public QStateMachine
{
    Q_OBJECT

    Q_PROPERTY(qreal progress WRITE setProgress READ progress)

public:
    QtMaterialComboBoxStateMachine(ContextMenu* menu, QtMaterialComboBox* parent);
    ~QtMaterialComboBoxStateMachine();

    void setLabel(QtMaterialComboBoxLabel* label);

    inline void setProgress(qreal progress);
    inline qreal progress() const;

public slots:
    void setupProperties();

private:
    bool isEmpty();

private:
    Q_DISABLE_COPY(QtMaterialComboBoxStateMachine)

    QtMaterialComboBox* const m_cb;
    QState* const m_normalState;
    QState* const m_focusedState;
    QtMaterialComboBoxLabel* m_label;
    QPropertyAnimation* m_offsetAnimation;
    QPropertyAnimation* m_colorAnimation;
    qreal m_progress;
};

inline void QtMaterialComboBoxStateMachine::setProgress(qreal progress)
{
    m_progress = progress;
    m_cb->update();
}

inline qreal QtMaterialComboBoxStateMachine::progress() const
{
    return m_progress;
}

class QtMaterialComboBoxLabel : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(qreal scale WRITE setScale READ scale)
    Q_PROPERTY(QPointF offset WRITE setOffset READ offset)
    Q_PROPERTY(QColor color WRITE setColor READ color)
    Q_PROPERTY(QColor heightLabel READ heightLabel)

public:
    QtMaterialComboBoxLabel(QtMaterialComboBox* parent);

    void setLabel(const QString& text)
    {
        m_text = text;
        update();
    }

    QString text() const
    {
        return m_text;
    }

    inline void setScale(qreal scale);
    inline qreal scale() const;

    inline void setOffset(const QPointF& pos);
    inline QPointF offset() const;

    inline void setColor(const QColor& color);
    inline QColor color() const;

    int heightLabel() const;

protected:
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

private:
    Q_DISABLE_COPY(QtMaterialComboBoxLabel)

    QtMaterialComboBox* const m_cb;
    qreal m_scale;
    qreal m_posX;
    qreal m_posY;
    QColor m_color;
    QString m_text;
};

inline void QtMaterialComboBoxLabel::setScale(qreal scale)
{
    m_scale = scale;
    update();
}

inline qreal QtMaterialComboBoxLabel::scale() const
{
    return m_scale;
}

inline void QtMaterialComboBoxLabel::setOffset(const QPointF& pos)
{
    m_posX = pos.x();
    m_posY = pos.y();
    update();
}

inline QPointF QtMaterialComboBoxLabel::offset() const
{
    return QPointF(m_posX, m_posY);
}

inline void QtMaterialComboBoxLabel::setColor(const QColor& color)
{
    m_color = color;
    update();
}

inline QColor QtMaterialComboBoxLabel::color() const
{
    return m_color;
}
