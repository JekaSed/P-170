#include "communicator.h"
#include "src/network/config.h"
#include "src/network/json-observer.h"
#include "src/network/status-codes.h"
#include <src/network/request.h>

#include <QUrlQuery>
#include <QNetworkRequest>
#include <QJsonObject>

Communicator::Communicator(QObject* parent)
  : QObject{parent}
{
    auto* observer =
      new network::JsonObserver(QNetworkRequest(QUrl(QString("%1/api/r-170p").arg(config::r170DriverEndpoint()))),
                                std::chrono::milliseconds(3000), true, this);
    connect(observer, &network::JsonObserver::stateChanged, this, &Communicator::stateChanged);
}

void Communicator::sendRequest(const QString& driverAddress, const QJsonObject& change)
{
    qDebug() << "\n send Change \n";
    qDebug() << Q_FUNC_INFO << change << '\n';
    auto d = QUrl(QString("%1/api/r-170p").arg(config::r170DriverEndpoint()));
    QUrlQuery q;
    //    q.addQueryItem("deviceName", driverAddress);
    d.setQuery(q);
    const QNetworkRequest req(d);

    auto* replier = new network::Requester(this);
    replier->start(
      req, network::HttpMethod::MethodPatch, QJsonDocument(change),
      [change, driverAddress](const network::Response& r) {
          if (r.code != network::statusNoContent) {
              qWarning() << QString("[Artek] ошибка установки параметров устройства %1:").arg(driverAddress) << change
                         << r.code << QString(r.body) << r.error;
          }
      });
}
