#ifndef VOLUMEWIDGET_H
#define VOLUMEWIDGET_H

#include <QAbstractSlider>
#include <QWidget>

#include <src/cloud/cloud.h>

class VolumeWidget : public Cloud
{
    Q_OBJECT
public:
    explicit VolumeWidget(QWidget* parent = nullptr);
    void setVolume(const int volume);
    int getVolume() const;

    QJsonObject toJsonObj() const;
    void fromJsonObj(const QJsonObject& obj);

private:
    QAbstractSlider* m_slider;
    class LCDNumber* m_valueWidget;
    QAbstractButton* m_muteBt;
    int m_lastMutedValue;

signals:
    void volumeChanged(int);
};

#endif   // VOLUMEWIDGET_H
