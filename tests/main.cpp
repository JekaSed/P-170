#include "artek-widget.h"
#include <QList>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QtTest/QtTest>
#include <QtTest/qtestcase.h>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidget>
#include <chrono>
#include <qcompare.h>

class TestArtekWidget final : public QObject
{
    Q_OBJECT

    void showWidget(QWidget* msg, int ms = 500, int width = 120, int height = 100)
    {
        QWidget w;
        w.setObjectName("test window");
        auto lay = new QVBoxLayout();
        lay->addWidget(msg);
        w.resize(width, height);
        w.show();
        w.setLayout(lay);
        processEvents(ms);
    }

    void processEvents(int ms = 1000)
    {
        QEventLoop loop;
        QTimer::singleShot(ms, [&loop] {
            loop.exit();
        });
        loop.exec();
    }

private slots:
    void testTdrTable()
    {
        auto* player = new ArtekWidget();

        showWidget(player, 1000, 700, 900);
    };
};

QTEST_MAIN(TestArtekWidget)
#include "main.moc"
