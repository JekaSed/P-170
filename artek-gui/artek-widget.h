#ifndef ARTEKWIDGET_H
#define ARTEKWIDGET_H

#include "src/frequency/frequency-widget.h"
#include <QWidget>

class ArtekWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ArtekWidget(QWidget* parent = nullptr);

private:
    FrequencyWidget* m_freqWidget;

signals:
};

#endif   // ARTEKWIDGET_H
