#include "http-server.h"
#include "llhttp/include/llhttp.h"
#include "src/network/request.h"
#include "src/network/status-codes.h"
#include <memory>
#include <qdebug.h>
#include <qfileinfo.h>
#include <qglobal.h>
#include <qhash.h>
#include <qmimedatabase.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qstringview.h>
#include <qtcpserver.h>
#include <qtcpsocket.h>
#include <qthread.h>
#include <qurl.h>
#include <utility>
//#include "llhttp.h"

namespace network {

namespace {

std::string mimeTypeForExt(const QFileInfo& fileExt)
{
    QMimeDatabase d;
    return d.mimeTypeForFile(fileExt).name().toStdString();
}

QString statusText(int code)
{
    switch (code) {
    case statusOk:
        return "OK";
    case statusNotFound:
        return "Not Found";
    case statusNoContent:
        return "No content";
    case statusCreated:
        return "Created";
    case statusMethodNotAllowed:
        return "Method not Allowed";
    case statusUnprocessableEntity:
        return "Unprocessable entity";
    case statusRequestTimeout:
        return "Request timeout";
    case statusAccepted:
        return "Accepted";
    case statusBadRequest:
        return "Bad request";
    case statusUnauthorized:
        return "Unauthorized";
    case statusNotAcceptable:
        return "Not Acceptable";
    case statusInternalServerError:
        return "Internal Server Error";

    default:
        return "Unknown";
    }
}

void writeHeaders(const Headers& headers, QByteArray& out)
{
    for (const auto& p : headers) {
        out += p.first.data();
        out += ": ";
        out += p.second.data();
        out += "\r\n";
    }
}

QByteArray makeAsk(const HttpResponse& resp)
{
    auto out = QString("HTTP/1.1 %1 %2\r\n").arg(resp.status).arg(statusText(resp.status)).toLatin1();
    out += "Server: GUI-http-server\r\n";
    out += "Connection: close\r\n";
    writeHeaders(resp.headers, out);
    if (!resp.body.isEmpty()) {
        if (resp.headers.find("Content-Type") == resp.headers.end()) {
            out += "Content-Type: application/json;charset=utf-8\r\n";
        }
        out += QString("Content-Length: %1\r\n").arg(resp.body.size()).toLatin1();

        // body
        out += "\r\n";
        out += resp.body;
        out += "\r\n";
    }

    return out;
}

HttpMethod fromReq(llhttp_method_t m)
{
    switch (m) {
    case HTTP_DELETE:
        return HttpMethod::MethodDelete;
    case HTTP_GET:
        return HttpMethod::MethodGet;
    case HTTP_POST:
        return HttpMethod::MethodPost;
    case HTTP_PUT:
        return HttpMethod::MethodPut;
    case HTTP_PATCH:
        return HttpMethod::MethodPatch;
    default:
        return HttpMethod::MethodGet;
    }
}

class HttpParser
{
    llhttp_t m_parser{};
    llhttp_settings_t m_settings{};
    QByteArray m_body;
    QByteArray m_url;
    std::string m_curHeaderName;
    std::string m_curHeaderValue;
    Headers m_headers;

public:
    static int onBodyData(llhttp_t* httpParser, const char* at, std::size_t size)
    {
        auto* self = static_cast<HttpParser*>(httpParser->data);
        self->m_body = QByteArray(at, size);
        return HPE_OK;
    }

    static int onHeaderName(llhttp_t* httpParser, const char* at, std::size_t size)
    {
        auto* self = static_cast<HttpParser*>(httpParser->data);
        self->m_curHeaderName.append(at, size);
        return HPE_OK;
    }

    static int onHeaderValue(llhttp_t* httpParser, const char* at, std::size_t size)
    {
        auto* self = static_cast<HttpParser*>(httpParser->data);
        self->m_curHeaderValue.append(at, size);
        return HPE_OK;
    }

    static int onHeaderComplete(llhttp_t* httpParser)
    {
        auto* self = static_cast<HttpParser*>(httpParser->data);

        assert(self->m_curHeaderName.size() > 0);   // NOLINT
        self->m_headers[self->m_curHeaderName] = self->m_curHeaderValue;
        self->m_curHeaderName.clear();
        self->m_curHeaderValue.clear();

        return HPE_OK;
    }

    static int onUrl(llhttp_t* httpParser, const char* at, std::size_t size)
    {
        auto* self = static_cast<HttpParser*>(httpParser->data);
        self->m_url.append(at, size);
        return HPE_OK;
    }

    HttpParser()
    {
        llhttp_settings_init(&m_settings);
        m_settings.on_body = onBodyData;
        m_settings.on_header_field = onHeaderName;
        m_settings.on_header_value = onHeaderValue;
        m_settings.on_url = onUrl;

        m_settings.on_header_value_complete = onHeaderComplete;

        llhttp_init(&m_parser, HTTP_REQUEST, &m_settings);
        m_parser.data = this;
    }

    bool parse(const QByteArray& rawRequest, HttpRequest& req)
    {
        if (llhttp_execute(&m_parser, rawRequest.data(), rawRequest.size()) != HPE_OK) {
            return false;
        }
        req.method = fromReq(static_cast<llhttp_method_t>(m_parser.method));
        req.url = QUrl::fromEncoded(m_url);
        req.body = std::move(m_body);
        req.headers = std::move(m_headers);
        return true;
    }
};

}   // namespace

HttpResponse SimpleHttpServer::makePlainTextResponse(const QString& text, int statusCode)
{
    HttpResponse resp;
    resp.status = statusCode;
    resp.headers["Content-Type"] = "text/plain; charset=utf-8";
    resp.body = text.toUtf8();
    return resp;
}

class SimpleHttpServer::Impl : public QObject
{
    friend class SimpleHttpServer;

    Q_OBJECT

public:
    explicit Impl()
      : m_server(new QTcpServer)
    {}

    void parse(QTcpSocket* s)
    {
        HttpParser parser;
        const auto rawRequest = s->readAll();
        HttpRequest req;
        if (!parser.parse(rawRequest, req)) {
            qWarning() << "failed to parse http request" << rawRequest;
            s->deleteLater();
            return;
        }
        const auto it = std::as_const(m_routes).find(req.url.path());
        if (it == m_routes.cend()) {
            auto out =
              makeAsk(makePlainTextResponse(QString("Ресурс \"%1\" не найден").arg(req.url.path()), statusNotFound));
            s->write(out);
            return;
        }
        const auto resp = it->second(req);
        auto out = makeAsk(resp);
        s->write(out);
    }

    void start()
    {
        moveToThread(&m_thread);
        m_server->moveToThread(&m_thread);
        QObject::connect(m_server.get(), &QTcpServer::newConnection, this, [this] {
            QTcpSocket* socket = m_server->nextPendingConnection();
            socket->moveToThread(&m_thread);
            QObject::connect(socket, &QTcpSocket::disconnected, socket, &QObject::deleteLater);
            QObject::connect(socket, &QTcpSocket::readyRead, this, [socket, this] {
                parse(socket);
            });
        });
        m_thread.start();
    }

    ~Impl()
    {
        m_server->deleteLater();
        m_thread.quit();
        m_thread.wait();
    }

public slots:

    void listen(quint16 p)
    {
        qDebug() << "http serve on " << p << m_server->listen(QHostAddress::Any, p);
    }

private:
    std::unique_ptr<QTcpServer> m_server;
    QThread m_thread;
    std::unordered_map<QString, SimpleHttpServer::Handler> m_routes;
};

SimpleHttpServer::SimpleHttpServer(QObject* parent)
  : QObject(parent)
  , m_impl(new Impl())
{
    m_impl->start();
}

SimpleHttpServer::~SimpleHttpServer() = default;

void SimpleHttpServer::serve(quint16 port)
{
    QMetaObject::invokeMethod(m_impl.get(), "listen", Q_ARG(quint16, port));
}

quint16 SimpleHttpServer::port() const
{
    return m_impl->m_server->serverPort();
}

void SimpleHttpServer::addRoute(const QString& path, Handler&& handler)
{
    qDebug() << "addRoute" << path;
    m_impl->m_routes.emplace(path, std::move(handler));
}

void SimpleHttpServer::publicFile(const QString& path, const QFileInfo& f)
{
    addRoute(path, [f](const HttpRequest&) {
        QFile fl(f.filePath());
        if (!fl.open(QIODevice::ReadOnly)) {
            qWarning() << "failed to open file" << f;
            return makePlainTextResponse(QString("не удалось прочитать %1: %2").arg(f.filePath(), fl.errorString()),
                                         statusInternalServerError);
        }
        HttpResponse resp;
        resp.body = fl.readAll();
        resp.headers["Content-Type"] = mimeTypeForExt(f);
        resp.status = statusOk;
        return resp;
    });
}

}   // namespace network

#include "http-server.moc"
