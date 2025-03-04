#ifndef ARTEKWIDGET_H
#define ARTEKWIDGET_H

#include "delayed.h"
#include "src/frequency/frequency-widget.h"
#include "src/volume-widget.h"
#include "src/workmode-widget.h"
#include <QJsonObject>
#include <QMap>
#include <QTimer>
#include <QWidget>

class ArtekWidget final
  : public QWidget
  , public Delayed<QJsonObject>
{
    Q_OBJECT
public:
    explicit ArtekWidget(QWidget* parent = nullptr);

    QJsonObject toJsonObj() const;
    void fromJsonObj(const QJsonObject& obj);
    static bool isValidJson(const QJsonObject& obj);

private:
    FrequencyWidget* m_freqWidget;
    VolumeWidget* m_volWidget;
    WorkModeWidget* m_wmWidget;
    QMap<QString, QJsonObject> m_changes;
    QTimer m_lastSendTimer;

private:
    void sendChange(const QString& key, const QJsonObject& value);

private slots:
    void sendAll();

signals:
    void changed(const QJsonObject&);

    // Delayed interface
public:
    void setNewState(const QJsonObject& newState) override;
};

#endif   // ARTEKWIDGET_H
