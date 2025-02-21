#pragma once

#include "src/theme/theme.h"
#include <QPushButton>
#include <qcolor.h>

class MenuButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MenuButton(const QIcon& icon);

public slots:
    void setIconSelected(bool checked);
    void setBadgeCircleVisible(bool visible);
    void setBadgeNum(int value);
    void setReactOnHover(bool reactOnHover);

    void setIconColor(QColor color);

    int badgeNum() const noexcept;

signals:
    void doubleClicked();

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void paintEvent(QPaintEvent* e) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent*) override;

private:
    QString valueToString() const noexcept;
    int m_value{};
    bool m_reactOnHover{true};
    QColor m_iconColor{theme::iconColor()};
    bool m_needDrawBadgeCircle{true};
    class QtMaterialRippleOverlay* m_overlay;
};
