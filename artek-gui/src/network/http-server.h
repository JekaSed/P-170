#pragma once

#include <functional>

#include <qbytearrayview.h>
#include <qfileinfo.h>
#include <qglobal.h>
#include <qobject.h>
#include <qurl.h>

#include "src/network/status-codes.h"
#include "request.h"

namespace network {

struct HttpRequest
{
    Headers headers;
    HttpMethod method;
    QByteArray body;
    QUrl url;
};

struct HttpResponse
{
    Headers headers;
    QByteArray body{R"({"status":"Ok"})"};
    int status{statusOk};
};

class SimpleHttpServer : public QObject
{
    Q_OBJECT
public:
    // пользовательский обработчик. Вызывается не в графическом потоке
    // если требуется работать с виджетами надо кидать сигнал из обработчика
    using Handler = std::function<HttpResponse(const HttpRequest&)>;

    explicit SimpleHttpServer(QObject* parent = nullptr);
    ~SimpleHttpServer() override;
    void serve(quint16 port = 0);
    quint16 port() const;

    void addRoute(const QString& path, Handler&& handler);

    void publicFile(const QString& path, const QFileInfo& f);

    static HttpResponse makePlainTextResponse(const QString& text, int statusCode);

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}   // namespace network

//NOLINTNEXTLINE
Q_DECLARE_METATYPE(network::HttpResponse)
