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
    QJsonObject toJsonObj() const;
    void fromJsonObj(const QJsonObject& obj);

private:
    FreqEditWidget* m_freqEditWidget;
signals:
    void freqChanged(const QJsonObject& obj);
};

#endif   // FREQUENCYWIDGET_H
