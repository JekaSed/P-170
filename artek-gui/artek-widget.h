#ifndef ARTEKWIDGET_H
#define ARTEKWIDGET_H

#include "src/frequency/frequency-widget.h"
#include "src/volume-widget.h"
#include "src/workmode-widget.h"
#include <QWidget>

class ArtekWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ArtekWidget(QWidget* parent = nullptr);

    QJsonObject toJsonObj() const;
    void fromJsonObj(const QJsonObject& obj);

private:
    FrequencyWidget* m_freqWidget;
    VolumeWidget* m_volWidget;
    WorkModeWidget* m_wmWidget;

signals:
};

#endif   // ARTEKWIDGET_H
