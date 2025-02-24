#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <QAction>
#include <QPropertyAnimation>
#include <QTimer>
#include <QScrollArea>
#include <qevent.h>

class ContextMenu : public QWidget
{
    Q_OBJECT
public:
    explicit ContextMenu(QWidget* parent);
    ~ContextMenu() override;

    void setActions(const QList<QAction*>& actionsList);
    void addActions(const QList<QAction*>& actionsList);
    void setColor(const QColor& color);
    void setFont(const QFont& font);
    void setMinItemHeight(int height);
    void setMinItemWidth(int width);
    void setBorderRadius(int radius);

signals:
    void aboutToShow();
    void showed();
    void hided();

private:
    void addMenuItem(QAction* action);
    QPoint calcZeroPoint(const QPoint& givenGlobalPosition);
    void updatePosition(const QPoint& menuGlobalPosition);
    void saveMenuPos(const QPoint&);
    QPoint restoreMenuPos();
    void removeEventFilter();
    void defineEventFilter();
    void slowHide();
    void slowShow();

private:
    QVBoxLayout* m_mainLy;
    QVBoxLayout* m_btLy;
    QWidget* m_mainWindow{nullptr};
    QPoint m_savedPoint;
    QScrollArea* m_scroll;
    QPropertyAnimation m_animateToggle;
    int m_minItemHeight{50};
    int m_minItemWidth{150};
    int m_radius{4};

public slots:
    void showMenu(const QPoint& menuGlobalPosition = QCursor::pos());
    void closeRequest();

private slots:
    void updateCurrentPosition();

protected:
    bool event(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

    // QWidget interface
public:
    QSize minimumSizeHint() const override;
};

#endif   // CONTEXTMENU_H
