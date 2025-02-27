#include <QThread>
#include <QTimerEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <chrono>
#include <qfuture.h>
#include <qmutex.h>
#include <qstringliteral.h>
#include <qthread.h>
#include <qtimer.h>
#include <qwaitcondition.h>
#include <QTimer>
#include <unordered_map>
#include <utility>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>

#include "request.h"
#include "src/network/status-codes.h"

namespace network {

namespace {

const QString appJson("application/json");
const QByteArray httpPatch("PATCH");
const QByteArray authHeader("Authorization");

}   // namespace

Reply::Reply(QObject* parent)
  : QObject(parent)
{}

DocumentLoader::DocumentLoader(const QNetworkRequest& req, QObject* parent)
  : network::Reply(parent)
  , m_req(req)
{}

void DocumentLoader::ready(network::Response resp)
{
    if (resp.code != network::statusOk) {
        QTimer::singleShot(std::chrono::milliseconds(300), this, &DocumentLoader::start);
        return;
    }
    emit loaded(QJsonDocument::fromJson(resp.body));
    deleteLater();
}

void DocumentLoader::start()
{
    network::get(this, m_req);
}

Requester::Requester(QObject* parent)
  : network::Reply(parent)
{}

void Requester::start(const QNetworkRequest& req, HttpMethod method, const QJsonDocument& doc,
                      Requester::ResponseHandler&& h)
{
    m_handler = std::move(h);
    switch (method) {
    case HttpMethod::MethodGet:
        get(this, req);
        break;
    case HttpMethod::MethodPost:
        post(this, req, doc);
        break;
    case HttpMethod::MethodPut:
        put(this, req, doc);
        break;
    case HttpMethod::MethodPatch:
        patch(this, req, doc);
        break;
    case HttpMethod::MethodDelete:
        deleteReq(this, req);
        break;
    }
}

void Requester::ready(network::Response resp)
{
    emit finished(resp);
    if (m_handler != nullptr) {
        m_handler(resp);
    }
    deleteLater();
}

RetryRequester::RetryRequester(const QNetworkRequest& req, HttpMethod method, int expectedCode, int repeats,
                               const QJsonDocument& doc, QObject* parent)
  : network::Reply(parent)
  , m_req(req)
  , m_method(method)
  , m_doc(doc)
  , m_expectedCode(expectedCode)
  , m_repeats(repeats)
{}

void RetryRequester::ready(network::Response resp)
{
    if (resp.code == m_expectedCode) {
        emit done({});
        deleteLater();
        return;
    }

    qWarning() << "Retrying request" << m_req.url().toString() << QString(resp.body) << "with code " << resp.code
               << "expected code" << m_expectedCode << "left try's"
               << (m_repeats < 0 ? QString{"∞"} : QString::number(m_repeats));

    if (m_repeats-- == 0) {
        cancel(resp.body);
        return;
    }
    QTimer::singleShot(std::chrono::milliseconds(300), this, &RetryRequester::start);
}

void RetryRequester::cancel(const QString& text)
{
    emit done(text);
    deleteLater();
}

void RetryRequester::start()
{
    switch (m_method) {
    case HttpMethod::MethodGet:
        get(this, m_req);
        break;
    case HttpMethod::MethodPost:
        post(this, m_req, m_doc);
        break;
    case HttpMethod::MethodPut:
        put(this, m_req, m_doc);
        break;
    case HttpMethod::MethodPatch:
        patch(this, m_req, m_doc);
        break;
    case HttpMethod::MethodDelete:
        deleteReq(this, m_req);
        break;
    }
}

ChainRequester::ChainRequester(QObject* parent)
  : QObject(parent)
{}

void ChainRequester::start()
{
    processNextRequest();
}

void ChainRequester::finish(QString error)
{
    emit done(error);
    deleteLater();
}

void ChainRequester::processNextRequest()
{
    if (m_requests.empty()) {
        finish({});
        return;
    }

    const Params param = m_requests.takeFirst();
    if (param.delay.count() > 0) {
        QTimer::singleShot(param.delay.count(), this, [this] {
            processNextRequest();
        });
        return;
    }

    RetryRequester* r = new RetryRequester(param.req, param.method, param.expectedCode, 0, param.doc, this);
    connect(r, &RetryRequester::done, this, [this](const QString& error) {
        if (error.isEmpty()) {
            processNextRequest();
        } else {
            finish(error);
        }
    });
    r->start();
}

void ChainRequester::addRequest(const QNetworkRequest& req, HttpMethod method, int expectedCode,
                                const QJsonDocument& doc)
{
    m_requests.append({req, method, expectedCode, doc});
}

void ChainRequester::addPause(std::chrono::milliseconds duration)
{
    Params p{};
    p.delay = duration;
    m_requests.append(p);
}

namespace {
Headers readHeaders(QNetworkReply* r)
{
    Headers res;
    for (const QPair<QByteArray, QByteArray>& head : r->rawHeaderPairs()) {
        res.emplace(head.first.toStdString(), head.second);
    }
    return res;
}

Response fromReply(QNetworkReply* reply, RequestContext ctx)
{
    Response res;
    res.headers = readHeaders(reply);
    res.code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    res.ctx = std::move(ctx);
    res.body = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
        res.error = reply->errorString();
    }
    return res;
}

}   // namespace

class NetworkAccess : public QObject
{
    Q_OBJECT

    struct ReplyContext
    {
        int timerID;
        Reply* replier;
        RequestContext ctx;
    };

public:
    NetworkAccess()
      : m_manager(this)
    {
        connect(&m_manager, &QNetworkAccessManager::finished, this, &NetworkAccess::deleteReply);
    }

public slots:

    void sendGet(Reply* replier, QNetworkRequest request)
    {
        insertHeaders(request);
        QNetworkReply* reply = m_manager.get(request);
        RequestContext ctx;
        ctx.method = HttpMethod::MethodGet;
        ctx.req = request;
        insertReply(reply, replier, ctx);
    }

    void sendDelete(Reply* replier, QNetworkRequest request)
    {
        insertHeaders(request);
        QNetworkReply* reply = m_manager.deleteResource(request);
        RequestContext ctx;
        ctx.method = HttpMethod::MethodDelete;
        ctx.req = request;
        insertReply(reply, replier, ctx);
    }

    void sendPost(Reply* replier, QNetworkRequest request, QByteArray data)
    {
        insertHeaders(request);
        RequestContext ctx;
        ctx.method = HttpMethod::MethodPost;
        ctx.req = request;
        auto* reply = m_manager.post(request, data);
        insertReply(reply, replier, ctx);
    }

    void sendPut(Reply* replier, QNetworkRequest request, QByteArray data)
    {
        insertHeaders(request);
        RequestContext ctx;
        ctx.method = HttpMethod::MethodPut;
        ctx.req = request;
        auto* reply = m_manager.put(request, data);
        insertReply(reply, replier, ctx);
    }

    void sendPatch(Reply* replier, QNetworkRequest request, QByteArray data)
    {
        insertHeaders(request);
        RequestContext ctx;
        ctx.method = HttpMethod::MethodPatch;
        ctx.req = request;
        auto* reply = m_manager.sendCustomRequest(request, httpPatch, data);
        insertReply(reply, replier, ctx);
    }

    void setAccessToken(const QString& t)
    {
        m_accessToken = QString("Bearer %1").arg(t).toLocal8Bit();
    }

    void setAccessToken(const QByteArray& t)
    {
        m_accessToken = t;
    }

public:
    QByteArray accessToken() const
    {
        return m_accessToken;
    }

private slots:

    void deleteReply(QNetworkReply* reply)
    {
        const auto it = m_reqs.find(reply);
        if (it == m_reqs.end()) {
            return;
        }
        const auto& req = it->second;
        killTimer(req.timerID);

        if (req.replier != nullptr) {
            const Response resp{fromReply(reply, req.ctx)};
            QMetaObject::invokeMethod(req.replier, "ready", Qt::QueuedConnection, Q_ARG(network::Response, resp));
        }
        m_reqs.erase(it);
        reply->deleteLater();
    }

private:
    void insertHeaders(QNetworkRequest& request) const
    {
        request.setHeader(QNetworkRequest::ContentTypeHeader, appJson);
        if (!m_accessToken.isEmpty()) {
            request.setRawHeader(authHeader, m_accessToken);
        }
    }

    void insertReply(QNetworkReply* reply, Reply* replier, RequestContext ctx)
    {
        constexpr auto timeout{std::chrono::seconds(10)};
        ReplyContext rCtx;
        rCtx.timerID = startTimer(timeout);
        rCtx.replier = replier;
        rCtx.ctx = std::move(ctx);
        m_reqs.emplace(reply, rCtx);
    }

    QNetworkReply* findByID(int timerID) const
    {
        const auto it = std::find_if(m_reqs.begin(), m_reqs.end(), [timerID](const auto& pair) {
            return pair.second.timerID == timerID;
        });

        if (it != m_reqs.end()) {
            return it->first;
        }
        return nullptr;
    }

    QNetworkAccessManager m_manager;
    QByteArray m_accessToken;

    std::unordered_map<QNetworkReply*, ReplyContext> m_reqs;

    // QObject interface
protected:
    void timerEvent(QTimerEvent* event) override
    {
        auto* reply = findByID(event->timerId());
        if (reply != nullptr) {
            reply->abort();
        }
    }
};

namespace {

NetworkAccess* startNetworkThread()
{
    static NetworkAccess netManager;
    return &netManager;
}

NetworkAccess* manager()
{
    return startNetworkThread();
}

}   // namespace

void get(Reply* replier, const QNetworkRequest& request)
{
    QMetaObject::invokeMethod(manager(), "sendGet", Q_ARG(Reply*, replier), Q_ARG(QNetworkRequest, request));
}

void post(Reply* replier, const QNetworkRequest& request, const QJsonDocument& doc)
{
    manager()->sendPost(replier, request, doc.toJson(QJsonDocument::JsonFormat::Compact));
}

void put(Reply* replier, const QNetworkRequest& request, const QJsonDocument& doc)
{
    manager()->sendPut(replier, request, doc.toJson(QJsonDocument::JsonFormat::Compact));
}

void patch(Reply* replier, const QNetworkRequest& request, const QJsonDocument& doc)
{
    manager()->sendPatch(replier, request, doc.toJson(QJsonDocument::JsonFormat::Compact));
}

void deleteReq(Reply* replier, const QNetworkRequest& request)
{
    manager()->sendDelete(replier, request);
}

void setUserToken(const QString& accessToken)
{
    QMetaObject::invokeMethod(manager(), "setAccessToken", Q_ARG(QString, accessToken));
}

QFuture<Response> getSync(const QNetworkRequest& request)
{
    auto* r = new Requester;
    r->start(request, network::HttpMethod::MethodGet);
    return QtFuture::connect(r, &Requester::finished);
}

QFuture<Response> postSync(const QNetworkRequest& request, const QJsonDocument& doc)
{
    auto* r = new Requester;
    r->start(request, network::HttpMethod::MethodPost, doc);
    return QtFuture::connect(r, &Requester::finished);
}

QFuture<Response> putSync(const QNetworkRequest& request, const QJsonDocument& doc)
{
    auto* r = new Requester;
    r->start(request, network::HttpMethod::MethodPut, doc);
    return QtFuture::connect(r, &Requester::finished);
}

QFuture<Response> patchSync(const QNetworkRequest& request, const QJsonDocument& doc)
{
    auto* r = new Requester;
    r->start(request, network::HttpMethod::MethodPatch, doc);
    return QtFuture::connect(r, &Requester::finished);
}

}   // namespace network

#include "request.moc"
