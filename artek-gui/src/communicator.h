#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <QObject>

class Communicator : public QObject
{
    Q_OBJECT
public:
    explicit Communicator(QObject* parent = nullptr);

public slots:
    void sendRequest(const QString& driverAddress, const QJsonObject& change);

signals:
    void stateChanged(const QJsonDocument&);
};

#endif   // COMMUNICATOR_H
