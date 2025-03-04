#ifndef QTMATERIALCHECKBOX_H
#define QTMATERIALCHECKBOX_H

#include "lib/qtmaterialcheckable.h"

class QtMaterialCheckBoxPrivate;

class QtMaterialCheckBox : public QtMaterialCheckable
{
    Q_OBJECT

public:
    explicit QtMaterialCheckBox(int size = 24, QWidget* parent = 0);
    ~QtMaterialCheckBox();

private:
    Q_DISABLE_COPY(QtMaterialCheckBox)
    Q_DECLARE_PRIVATE(QtMaterialCheckBox)
};

#endif
