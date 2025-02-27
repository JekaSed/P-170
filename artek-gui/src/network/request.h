#pragma once

#include <chrono>
#include <functional>
#include <qfuture.h>
#include <qjsondocument.h>
#include <qobject.h>
#include <string>
#include <unordered_map>

#include <QByteArray>
#include <QObject>
#include <QMetaType>
#include <QNetworkRequest>
#include <QJsonDocument>

namespace network {

enum class HttpMethod
{
    MethodGet,
    MethodPost,
    MethodPut,
    MethodPatch,
    MethodDelete,
};

struct RequestContext
{
    QNetworkRequest req;
    HttpMethod method;
};

using Headers = std::unordered_map<std::string, std::string>;

struct Response
{
    RequestContext ctx;

    QByteArray body;
    int code;
    Headers headers;
    QString error;
};

/*!
 * @brief Интерфейс для получения ответа на rest-запрос
 *
 */
class Reply : public QObject
{
    Q_OBJECT

public:
    Reply(QObject* parent = nullptr);

public slots:
    virtual void ready(Response) = 0;
};

/**
 * @brief Класс для одноразовой загрузки данных по HTTP метод GET, после успешной выгрузки
 * вызовет сигнал `loaded` и автоматически удалится. Будет повторять попытки пересылки при неудаче
 */
class DocumentLoader final : public Reply
{
    Q_OBJECT

public:
    explicit DocumentLoader(const QNetworkRequest& req, QObject* parent = nullptr);
    void start();

signals:
    void loaded(const QJsonDocument& doc);

public slots:
    void ready(network::Response resp) override;

private:
    const QNetworkRequest m_req;
};

/**
 * @brief Одноразово посылает запрос, по завершению обработки испускает сигнал о завершении и автоматически удаляется
 *
 */
class Requester final : public Reply
{
    Q_OBJECT

public:
    using ResponseHandler = std::function<void(const network::Response& resp)>;
    explicit Requester(QObject* parent = nullptr);
    void start(const QNetworkRequest& req, HttpMethod method, const QJsonDocument& doc = {},
               ResponseHandler&& h = nullptr);

public slots:
    void ready(network::Response resp) override;

signals:
    void finished(network::Response);

private:
    ResponseHandler m_handler;
};

/**
 * @brief Будет повторять выполнение запроса
 *
 */
class RetryRequester final : public network::Reply
{
    Q_OBJECT

public:
    explicit RetryRequester(const QNetworkRequest& req, HttpMethod method, int expectedCode, int repeats = -1,
                            const QJsonDocument& doc = {}, QObject* parent = nullptr);
signals:
    void done(QString error);

public slots:
    void start();
    void cancel(const QString& err = "cancelled");
    void ready(network::Response resp) override;

private:
    const QNetworkRequest m_req;
    const HttpMethod m_method;
    const QJsonDocument m_doc;
    const int m_expectedCode;
    int m_repeats;
};

/**
 * @brief Последовательно выполняет цепочку запросов.
 * При получении ошибке завершает выполнение последующих запросов (прерывает цепочку)
 */
class ChainRequester final : public QObject
{
    Q_OBJECT

public:
    struct Params
    {
        QNetworkRequest req;
        HttpMethod method;
        int expectedCode;
        QJsonDocument doc;
        std::chrono::milliseconds delay = std::chrono::milliseconds{0};
    };

    explicit ChainRequester(QObject* parent = nullptr);
    void start();

    void addRequest(const QNetworkRequest& req, HttpMethod method, int expectedCode, const QJsonDocument& doc);
    void addPause(std::chrono::milliseconds duration);

signals:
    /**
     * @brief результат выполнения цепочки
     *
     * @param ok успешное завершение запросов
     */
    void done(QString error);

private:
    void finish(QString error);
    void processNextRequest();
    QList<Params> m_requests;
};

void get(Reply* replier, const QNetworkRequest& request);
void deleteReq(Reply* replier, const QNetworkRequest& request);
void post(Reply* replier, const QNetworkRequest& request, const QJsonDocument& doc);
void put(Reply* replier, const QNetworkRequest& request, const QJsonDocument& doc);
void patch(Reply* replier, const QNetworkRequest& request, const QJsonDocument& doc);
void setUserToken(const QString& accessToken);

QFuture<Response> getSync(const QNetworkRequest& request);
QFuture<Response> postSync(const QNetworkRequest& request, const QJsonDocument& doc);
QFuture<Response> putSync(const QNetworkRequest& request, const QJsonDocument& doc);
QFuture<Response> patchSync(const QNetworkRequest& request, const QJsonDocument& doc);

}   // namespace network

//NOLINTNEXTLINE
Q_DECLARE_METATYPE(network::Response)
//NOLINTNEXTLINE
Q_DECLARE_METATYPE(network::RequestContext)
