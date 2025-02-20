#include "qtmaterialcheckbox.h"
#include "qtmaterialcheckbox_p.h"
#include <QPropertyAnimation>

#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <QtStateMachine/QStateMachine>
#include <QtStateMachine/QState>
#include <QtStateMachine/QSignalTransition>
#else
#include <QStateMachine>
#include <QSignalTransition>
#include <QState>
#endif

#include "lib/qtmaterialcheckable_internal.h"

/*!
 *  \class QtMaterialCheckBoxPrivate
 *  \internal
 */

/*!
 *  \internal
 */
QtMaterialCheckBoxPrivate::QtMaterialCheckBoxPrivate(QtMaterialCheckBox* q)
  : QtMaterialCheckablePrivate(q)
{}

/*!
 *  \internal
 */
QtMaterialCheckBoxPrivate::~QtMaterialCheckBoxPrivate()
{}

/*!
 *  \internal
 */
void QtMaterialCheckBoxPrivate::init(int s)
{
    Q_Q(QtMaterialCheckBox);

    checkedIcon->setIconSize(s);
    uncheckedIcon->setIconSize(s);
    checkedState->assignProperty(checkedIcon, "iconSize", s);
    uncheckedState->assignProperty(checkedIcon, "iconSize", 0);

    QPropertyAnimation* animation;

    animation = new QPropertyAnimation(checkedIcon, "iconSize", q);
    animation->setDuration(300);
    uncheckedTransition->addAnimation(animation);

    animation = new QPropertyAnimation(checkedIcon, "iconSize", q);
    animation->setDuration(300);
    checkedTransition->addAnimation(animation);

    animation = new QPropertyAnimation(checkedIcon, "opacity", q);
    animation->setDuration(440);
    checkedTransition->addAnimation(animation);

    animation = new QPropertyAnimation(checkedIcon, "opacity", q);
    animation->setDuration(440);
    uncheckedTransition->addAnimation(animation);

    animation = new QPropertyAnimation(uncheckedIcon, "opacity", q);
    animation->setDuration(440);
    checkedTransition->addAnimation(animation);

    animation = new QPropertyAnimation(uncheckedIcon, "opacity", q);
    animation->setDuration(440);
    uncheckedTransition->addAnimation(animation);

    animation = new QPropertyAnimation(uncheckedIcon, "color", q);
    animation->setDuration(440);
    checkedTransition->addAnimation(animation);

    animation = new QPropertyAnimation(uncheckedIcon, "color", q);
    animation->setDuration(440);
    uncheckedTransition->addAnimation(animation);
}

/*!
 *  \class QtMaterialCheckBox
 */

QtMaterialCheckBox::QtMaterialCheckBox(int size, QWidget* parent)
  : QtMaterialCheckable(*new QtMaterialCheckBoxPrivate(this), parent)
{
    d_func()->init(size);
}

QtMaterialCheckBox::~QtMaterialCheckBox()
{}
