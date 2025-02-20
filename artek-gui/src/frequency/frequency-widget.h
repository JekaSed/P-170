#ifndef FREQUENCYWIDGET_H
#define FREQUENCYWIDGET_H

#include "src/frequency/FreqEditWidget.h"
#include <QWidget>
#include <src/cloud/cloud.h>

class FrequencyWidget : public Cloud
{
    Q_OBJECT
public:
    explicit FrequencyWidget(QWidget* parent = nullptr);

private:
    FreqEditWidget* m_freqEditWidget;
signals:
    void freqChanged(long int);
};

#endif   // FREQUENCYWIDGET_H
