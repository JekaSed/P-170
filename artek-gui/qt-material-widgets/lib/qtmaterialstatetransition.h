#ifndef QTMATERIALSTATETRANSITION_H
#define QTMATERIALSTATETRANSITION_H
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <QtStateMachine/QStateMachine>
#include <QtStateMachine/QAbstractTransition>
#else
#include <QAbstractTransition>
#endif
#include "lib/qtmaterialstatetransitionevent.h"

class QtMaterialStateTransition : public QAbstractTransition
{
    Q_OBJECT

public:
    QtMaterialStateTransition(QtMaterialStateTransitionType type);

protected:
    virtual bool eventTest(QEvent *event);
    virtual void onTransition(QEvent *);

private:
    QtMaterialStateTransitionType m_type;
};

#endif // QTMATERIALSTATETRANSITION_H
