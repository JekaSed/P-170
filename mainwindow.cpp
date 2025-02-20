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
  , m_radioWidget(new ArtekWidget(this))
{
    ui->setupUi(this);
    setMinimumSize(900, 700);

    setStyleSheet(theme::mainCSS());
    auto pal = qApp->palette();
    pal.setColor(QPalette::Link, theme::titleColor());
    qApp->setPalette(pal);
    qApp->setFont(theme::baseFont());

    setCentralWidget(m_radioWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    //    if (m_tdrEditor->closeRequest()) {
    //        event->ignore();
    //        return;
    //    };
    QMainWindow::closeEvent(event);
}
