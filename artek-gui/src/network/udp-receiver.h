#pragma once

#include <QObject>
#include <qglobal.h>
#include <qobject.h>
#include <qobjectdefs.h>

namespace network {

class DataListener : public QObject
{
    Q_OBJECT
public:
    DataListener(QObject* parent = nullptr);

public slots:
    virtual void dataReady(QByteArray) = 0;
};

void listenUdp(DataListener* listener, quint16 port);
void listenRtp(DataListener* listener, quint16 port);
void listenConstellation(DataListener* listener, quint16 port);

void listenTcp(DataListener* listener, quint16 port);
void listenMorseEcho(DataListener* listener, quint16 port);

void stop();

}   // namespace network
