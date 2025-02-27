#pragma once

#include <QString>

namespace config {

void setServerAddress(const QString& address);
QString serverAddress();
QString halEndpoint();
QString dnsEndpoint();
QString centerEndpoint();
QString stationEndpoint();
QString radioChannelEndpoint();
QString schedulerEndpoint();

QString journalEndpoint();
QString authEndpoint();
QString storageEndpoint();

QString radioDataEndpoint();
QString dataTransmitterEndpoint();
QString r170DriverEndpoint();
}   // namespace config
