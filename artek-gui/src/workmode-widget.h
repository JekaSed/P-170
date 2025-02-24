#ifndef WORKMODEWIDGET_H
#define WORKMODEWIDGET_H

#include "src/cloud/cloud.h"

#include <QComboBox>

class WorkModeWidget : public Cloud
{
    Q_OBJECT
public:
    WorkModeWidget(QWidget* p);

private:
    class WorkModeData* m_data;
    QComboBox* m_emissionCb;
    QComboBox* m_deviationCb;
    QComboBox* m_bitrateCb;

private:
    void updateDeviations();
    void updateBitrates();
};

#endif   // WORKMODEWIDGET_H
