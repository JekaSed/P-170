#ifndef ARTEKWIDGET_H
#define ARTEKWIDGET_H

#include "src/frequency/frequency-widget.h"
#include "src/volume-widget.h"
#include <QWidget>

class ArtekWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ArtekWidget(QWidget* parent = nullptr);

private:
    FrequencyWidget* m_freqWidget;
    VolumeWidget* m_volWidget;

signals:
};

#endif   // ARTEKWIDGET_H
