#pragma once
#include <QtGlobal>
#include <QColor>
#include "src/menu/contextmenu.h"

class QtMaterialComboBox;
class QtMaterialComboBoxStateMachine;
class QtMaterialComboBoxLabel;

class QtMaterialComboBoxPrivate
{
    Q_DISABLE_COPY(QtMaterialComboBoxPrivate)
    Q_DECLARE_PUBLIC(QtMaterialComboBox)

public:
    QtMaterialComboBoxPrivate(QtMaterialComboBox* q);
    virtual ~QtMaterialComboBoxPrivate();

    void init();

    QtMaterialComboBox* const q_ptr;
    QtMaterialComboBoxStateMachine* stateMachine;
    QtMaterialComboBoxLabel* label;
    ContextMenu* m_menu;
    QColor textColor;
    QColor labelColor;
    QColor inkColor;
    QColor inputLineColor;
    qreal labelFontSize;
    bool useThemeColors;
    bool ignoreWheel;
};
