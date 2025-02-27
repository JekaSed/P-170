#include "json-observer.h"

#include <chrono>
#include <utility>

#include <QTimer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QJsonValue>

#include "defer.h"
#include "request.h"

namespace network {

class JsonObserver::Impl final : public Reply
{
    friend class JsonObserver;   // for poll()

    Q_OBJECT
public:
    Impl(const QNetworkRequest& r, std::chrono::milliseconds t, bool startImmediately, JsonObserver* owner)
      : Reply(owner)
      , m_req(r)
      , m_timeout(t)
      , m_owner(owner)
    {
        if (startImmediately) {
            poll();
        }
    }

public slots:

    void ready(network::Response resp) override
    {
        const Defer defer([this] {
            QTimer::singleShot(m_timeout, this, SLOT(poll()));
        });
        QJsonParseError err{};
        auto doc = QJsonDocument::fromJson(resp.body, &err);
        if (err.error != QJsonParseError::NoError) {
            QJsonObject obj;
            obj[QStringLiteral("error")] = resp.error;
            obj[QStringLiteral("errorDescription")] = resp.body.data();
            if (obj != m_cached.object()) {
                qWarning() << resp.ctx.req.url().toString() << err.errorString() << resp.error;
                m_cached.setObject(obj);
                m_owner->emitNewState(m_cached);
            }
            return;
        }
        if (doc.isObject()) {
            auto obj = doc.object();
            auto cachedObj = m_cached.object();
            if (obj != cachedObj) {
                m_cached.swap(doc);
                m_owner->emitNewState(m_cached);
            }
        } else if (doc.isArray()) {
            auto obj = doc.array();
            auto cachedObj = m_cached.array();
            if (obj != cachedObj) {
                m_cached.swap(doc);
                m_owner->emitNewState(m_cached);
            }
        }
    }

private slots:

    void poll()
    {
        network::get(this, m_req);
    }

private:
    const QNetworkRequest m_req;
    const std::chrono::milliseconds m_timeout;
    JsonObserver* const m_owner;
    QJsonDocument m_cached;
};

JsonObserver::JsonObserver(const QNetworkRequest& request, std::chrono::milliseconds msPollTime, bool startImmediately,
                           QObject* parent)
  : QObject(parent)
  , m_impl(new Impl(request, msPollTime, startImmediately, this))
{}

JsonObserver::~JsonObserver() = default;

void JsonObserver::emitNewState(QJsonDocument doc) noexcept
{
    emit stateChanged(std::move(doc));
}

void JsonObserver::start()
{
    m_impl->poll();
}

}   // namespace network

#include "json-observer.moc"
