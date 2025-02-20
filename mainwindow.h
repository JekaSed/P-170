#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "artek-widget.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    ArtekWidget* m_radioWidget;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent* event) override;
};
#endif   // MAINWINDOW_H
