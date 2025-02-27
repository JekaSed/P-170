#include <memory>
#include <array>
#include <cmath>

#include <QUdpSocket>
#include <QThread>
#include <QDebug>
#include <QPointF>
#include <qglobal.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpoint.h>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextCodec>
#include <algorithm>
#include "src/gsl/pointers"

#include "udp-receiver.h"

#include "rtp/unpacker.h"

namespace network {

namespace {

QThread* start()
{
    static QThread udpThread;
    if (!udpThread.isRunning()) {
        udpThread.start();
    }
    return &udpThread;
}

class UdpReceiver : public QObject
{
    Q_OBJECT
public:
    UdpReceiver(DataListener* listener, quint16 port)
      : m_udpSocket(new QUdpSocket())
    {
        qDebug() << "start listening" << port << m_udpSocket->bind(QHostAddress::Any, port);

        connect(this, &UdpReceiver::dataReady, listener, &DataListener::dataReady);
        connect(listener, &QObject::destroyed, this, &UdpReceiver::deleteLater);
        connect(m_udpSocket, &QUdpSocket::readyRead, this, &UdpReceiver::readPendingDatagrams);
    }

    ~UdpReceiver() override
    {
        m_udpSocket->deleteLater();
        qDebug() << "stop listening" << m_udpSocket->localPort();
    }

    void move()
    {
        auto* work = start();
        m_udpSocket->moveToThread(work);
        this->moveToThread(work);
    }

signals:
    void dataReady(QByteArray);

private slots:

    void readPendingDatagrams()
    {
        QByteArray datagram;
        while (m_udpSocket->hasPendingDatagrams()) {
            int datagramSize = static_cast<int>(m_udpSocket->pendingDatagramSize());
            datagram.resize(datagramSize);
            m_udpSocket->readDatagram(datagram.data(), datagramSize);
            emit dataReady(datagram);
        }
    }

private:
    QUdpSocket* m_udpSocket;
};

class RtpReceiver final : public DataListener
{
    Q_OBJECT

public:
    RtpReceiver(DataListener* listener, quint16 port)
      : m_udp(new UdpReceiver(this, port))
      , m_unpacker(RtpUnpacker_Create(), RtpUnpacker_Destroy)
    {
        connect(listener, &QObject::destroyed, this, &RtpReceiver::deleteLater);
        connect(this, &RtpReceiver::dataUnpacked, listener, &DataListener::dataReady);
    }

    void move()
    {
        auto* work = start();
        m_udp->move();
        this->moveToThread(work);
    }

public slots:

    void dataReady(QByteArray data) override
    {
        //распаковка
        RtpUnpackerResult state = RtpUnpacker_Process(m_unpacker.get(), (const uint8_t*)data.data(), data.size());

        //если обнаружена ошибка распаковки
        if ((state.flags & RTP_EVENT_FORMAT_ERR) != 0U || state.dataOffset == -1) {
            qWarning() << "Ошибка распаковки RTP";
            return;
        }
        const auto unpacked = data.mid(state.dataOffset, state.dataSize);
        if (state.timestampSkip > 0) {
            qWarning() << " добавляем пропущенные отсчеты RTP" << state.timestampSkip;
            QByteArray res(std::clamp(state.timestampSkip, 0, 4096), 0x17);
            res += unpacked;
            emit dataUnpacked(res);
            return;
        }
        emit dataUnpacked(unpacked);
    }

signals:
    void dataUnpacked(QByteArray);

private:
    UdpReceiver* m_udp;

    using rtpDeleter = void (*)(RtpUnpacker*);
    std::unique_ptr<RtpUnpacker, rtpDeleter> m_unpacker;
};

class ConstellationReceiver final : public DataListener
{
    Q_OBJECT
public:
    ConstellationReceiver(DataListener* listener, quint16 port)
      : m_rtp(new RtpReceiver(this, port))
    {
        connect(listener, &QObject::destroyed, this, &ConstellationReceiver::deleteLater);
        connect(m_rtp, &RtpReceiver::dataUnpacked, this, &ConstellationReceiver::dataReady);
        connect(this, &ConstellationReceiver::constellationReady, listener, &DataListener::dataReady);
    }

    void move()
    {
        auto* work = start();
        m_rtp->move();
        this->moveToThread(work);
    }

signals:
    void constellationReady(QByteArray);

public slots:

    void dataReady(QByteArray samples) override
    {
        constexpr double PI = 3.14159265358979323846;
        constexpr float phase = 180.0F;

        const qint16* data = (const qint16*)samples.data();
        int size = samples.size() / sizeof(qint16);
        for (int i = 0; i < size; ++i) {
            auto value = data[i];
            const auto radians = (PI * value) / phase;
            auto vx = std::cos(radians);
            auto vy = (-1) * std::sin(radians);

            //вносим в очередь (циклически)
            m_pointIndex = (m_pointIndex + 1) % nPoints;
            m_points[m_pointIndex] = QPointF(vx, vy);

            //наращиваем счетчик
            ++m_recalcCounter;

            //принимаем решение об отправке сообщения
            if (m_recalcCounter == recalcLimit) {
                m_recalcCounter = 0;
                emit constellationReady(QByteArray::fromRawData((char*)m_points.data(), nPoints * sizeof(QPointF)));
            }
        }
    }

private:
    RtpReceiver* m_rtp;

    static constexpr auto nPoints{1024};
    static constexpr auto recalcLimit{256};

    std::array<QPointF, nPoints> m_points;
    int m_pointIndex{};
    int m_recalcCounter{};
};

class TcpReceiver : public QObject
{
    Q_OBJECT

public:
    TcpReceiver(DataListener* listener, quint16 port)
      : m_port(port)
    {
        connect(listener, &QObject::destroyed, this, &TcpReceiver::deleteLater);
        connect(this, &TcpReceiver::dataReady, listener, &DataListener::dataReady);
    }

    ~TcpReceiver() override
    {
        qDebug() << "stop listening" << m_port;
    }

    void run()
    {
        qDebug() << "start listening" << m_port << m_server->listen(QHostAddress::Any, m_port);
    }

    void move()
    {
        auto* work = start();
        moveToThread(work);
        m_server = new QTcpServer(work);
        connect(m_server, &QTcpServer::newConnection, this, &TcpReceiver::onConnect);
        connect(this, &QObject::destroyed, m_server, &QTcpSocket::deleteLater);
    }

signals:
    void dataReady(QByteArray);

private slots:

    void onConnect()
    {
        auto* socket = m_server->nextPendingConnection();
        connect(this, &QObject::destroyed, socket, &QTcpSocket::deleteLater);
        connect(socket, &QTcpSocket::readyRead, this, &TcpReceiver::readPendingDatagrams);
        qDebug() << "connected" << socket->peerPort() << socket->localPort();
    }

    void readPendingDatagrams()
    {
        auto* socket = qobject_cast<QTcpSocket*>(sender());
        emit dataReady(socket->readAll());
    }

private:
    const quint16 m_port;
    gsl::owner<QTcpServer*> m_server{};
};

class MorseReader final : public DataListener
{
    Q_OBJECT

public:
    MorseReader(DataListener* listener, quint16 port)
      : m_tcp(new TcpReceiver(this, port))
    {
        connect(listener, &QObject::destroyed, this, &MorseReader::deleteLater);
        connect(this, &MorseReader::morseReady, listener, &DataListener::dataReady);
    }

    void move()
    {
        auto* work = start();
        moveToThread(work);
        m_tcp->move();
        m_tcp->run();
    }

signals:
    void morseReady(QByteArray);

public slots:

    void dataReady(QByteArray data) override
    {
        static auto* c = QTextCodec::codecForName("cp1251");
        emit morseReady(QByteArray::fromStdString(c->toUnicode(data).toStdString()));
    }

private:
    TcpReceiver* m_tcp;
};

}   // namespace

DataListener::DataListener(QObject* parent)
  : QObject(parent)
{}

void listenUdp(DataListener* listener, quint16 port)
{
    gsl::owner<UdpReceiver*> receiver = new UdpReceiver(listener, port);
    receiver->move();
}

void listenRtp(DataListener* listener, quint16 port)
{
    gsl::owner<RtpReceiver*> receiver = new RtpReceiver(listener, port);
    receiver->move();
}

void listenConstellation(DataListener* listener, quint16 port)
{
    gsl::owner<ConstellationReceiver*> receiver = new ConstellationReceiver(listener, port);
    receiver->move();
}

void listenTcp(DataListener* listener, quint16 port)
{
    gsl::owner<TcpReceiver*> receiver = new TcpReceiver(listener, port);
    receiver->move();
    receiver->run();
}

void listenMorseEcho(DataListener* listener, quint16 port)
{
    gsl::owner<MorseReader*> receiver = new MorseReader(listener, port);
    receiver->move();
}

void stop()
{
    auto* thread = start();
    thread->quit();
    thread->wait(100);
}

}   // namespace network

#include "udp-receiver.moc"
