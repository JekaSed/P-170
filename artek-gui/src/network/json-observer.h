#pragma once
#include <chrono>
#include <memory>

#include <QObject>
#include <QNetworkRequest>
#include <QJsonValue>
#include <QJsonDocument>

namespace network {

class JsonObserver : public QObject
{
    Q_OBJECT
public:
    static constexpr auto defaultPollTime = std::chrono::milliseconds(300);

    explicit JsonObserver(const QNetworkRequest& request, std::chrono::milliseconds msPollTime = defaultPollTime,
                          bool startImmediately = true, QObject* parent = nullptr);
    void start();

    ~JsonObserver() override;
signals:
    void stateChanged(QJsonDocument);

private:
    void emitNewState(QJsonDocument) noexcept;
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}   // namespace network
