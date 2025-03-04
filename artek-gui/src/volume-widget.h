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

    QJsonObject toJsonObj() const;
    void fromJsonObj(const QJsonObject& obj);

private:
    QAbstractSlider* m_slider;
    class LCDNumber* m_valueWidget;
    QAbstractButton* m_muteBt;
    void emitChangedSignal();

signals:
    void volumeChanged(const QJsonObject& obj);
};

#endif   // VOLUMEWIDGET_H
