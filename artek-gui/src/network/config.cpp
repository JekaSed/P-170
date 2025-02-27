#include "config.h"

namespace config {

namespace {

struct Config
{
    QString serverEp;
};

// Config cfg{"http://127.0.0.1:50002"};
Config cfg{"http://gitlab.radio.voz:50002"};

}   // namespace

void setServerAddress(const QString& address)
{
    cfg.serverEp = QString("http://%1").arg(address);
}

QString serverAddress()
{
    return cfg.serverEp;
}

QString halEndpoint()
{
    return QString("%1/hal").arg(cfg.serverEp);
}

QString dnsEndpoint()
{
    return QString("%1/dns").arg(cfg.serverEp);
}

QString centerEndpoint()
{
    return QString("%1/center").arg(cfg.serverEp);
}

QString stationEndpoint()
{
    return QString("%1/station").arg(cfg.serverEp);
}

QString schedulerEndpoint()
{
    return QString("%1/scheduler").arg(cfg.serverEp);
}

QString radioChannelEndpoint()
{
    return QString("%1/radiochannel").arg(cfg.serverEp);
}

QString journalEndpoint()
{
    return QString("%1/log").arg(cfg.serverEp);
}

QString authEndpoint()
{
    return QString("%1/auth").arg(cfg.serverEp);
}

QString storageEndpoint()
{
    return QString("%1/storage").arg(cfg.serverEp);
}

QString radioDataEndpoint()
{
    // FIXME: change to localhost
    return QString("http://gitlab.radio.voz:50010");
}

QString dataTransmitterEndpoint()
{
    return QString("http://gitlab.radio.voz:50011");
}

QString r170DriverEndpoint()
{
    return QString("http://127.0.0.1");
}

}   // namespace config
