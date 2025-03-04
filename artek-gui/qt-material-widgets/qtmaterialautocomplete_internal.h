#ifndef QTMATERIALAUTOCOMPLETESTATEMACHINE_H
#define QTMATERIALAUTOCOMPLETESTATEMACHINE_H

#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <QtStateMachine/QStateMachine>
#include <QtStateMachine/QState>
#else
#include <QStateMachine>
#endif
#include "qtmaterialautocomplete.h"

class QtMaterialAutoCompleteStateMachine : public QStateMachine
{
    Q_OBJECT

public:
    explicit QtMaterialAutoCompleteStateMachine(QWidget *menu);
    ~QtMaterialAutoCompleteStateMachine();

signals:
    void shouldOpen();
    void shouldClose();
    void shouldFade();

private:
    Q_DISABLE_COPY(QtMaterialAutoCompleteStateMachine)

    QWidget *const m_menu;
    QState  *const m_closedState;
    QState  *const m_openState;
    QState  *const m_closingState;
};

#endif // QTMATERIALAUTOCOMPLETESTATEMACHINE_H
