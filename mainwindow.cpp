#include "mainwindow.h"
#include "./ui_mainwindow.h"
//#include "qdebug.h"

#include <QCloseEvent>
#include <QHBoxLayout>
#include <QPushButton>

#include "src/theme/theme.h"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_artek(new ArtekEditor(this))
{
    ui->setupUi(this);
    setMinimumSize(900, 700);

    setWindowTitle("Р160-П");

    setStyleSheet(theme::mainCSS());
    auto pal = qApp->palette();
    pal.setColor(QPalette::Link, theme::titleColor());
    qApp->setPalette(pal);
    qApp->setFont(theme::baseFont());

    setCentralWidget(m_artek);
    connect(m_artek, &ArtekEditor::fileChanged, this, [this](const QString& filename) {
        setWindowTitle(filename.isEmpty() ? "Новая сессия*" : filename);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (m_artek->closeRequest()) {
        event->ignore();
        return;
    };
    QMainWindow::closeEvent(event);
}
