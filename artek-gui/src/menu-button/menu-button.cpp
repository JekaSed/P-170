#include "menu-button.h"
#include "lib/qtmaterialripple.h"
#include "lib/qtmaterialrippleoverlay.h"
#include "src/theme/theme.h"

#include <QToolTip>
#include <QPainter>
#include <QPainterPath>

const QString maxValue(QLatin1String("99+"));

MenuButton::MenuButton(const QIcon& icon)
  : m_overlay(new QtMaterialRippleOverlay(this))
{
    setStyleSheet(
      QString("QPushButton{border:none;background:transparent;color:%3} QToolTip {border: 1px solid %1;padding: "
              "5px;border-radius: 3px;opacity: 200; color:%2;}")
        .arg(theme::iconHighLightColor().name(),   //
             theme::textColor().name(),            //
             theme::titleColor().name()));
    setIcon(theme::changeColor(icon, m_iconColor));
    setCursor(Qt::PointingHandCursor);
    setIconSize(QSize(65, 65));
    setCheckable(true);
    connect(this, &QPushButton::toggled, this, &MenuButton::setIconSelected);
}

void MenuButton::setBadgeCircleVisible(bool visible)
{
    if (m_needDrawBadgeCircle == visible) {
        return;
    }
    m_needDrawBadgeCircle = visible;
    update();
}

void MenuButton::setIconSelected(bool checked)
{
    if (!m_reactOnHover) {
        return;
    }
    setIcon(theme::changeColor(icon(), checked ? theme::iconHighLightColor() : m_iconColor));
}

void MenuButton::setBadgeNum(int value)
{
    if (value != m_value) {
        m_value = value;
        update();
    }
}

void MenuButton::setReactOnHover(bool reactOnHover)
{
    m_reactOnHover = reactOnHover;
}

void MenuButton::setIconColor(QColor color)
{
    m_iconColor = color;
    setIcon(theme::changeColor(icon(), m_iconColor));
    update();
}

int MenuButton::badgeNum() const noexcept
{
    return m_value;
}

void MenuButton::enterEvent(QEnterEvent* event)
{
    if (!isEnabled()) {
        return;
    }
    QPushButton::enterEvent(event);
    setIconSelected(true);
    const auto r = rect();
    QToolTip::showText(mapToGlobal(r.bottomLeft()), toolTip(), this, r);
}

void MenuButton::leaveEvent(QEvent* event)
{
    if (!isEnabled()) {
        return;
    }
    QPushButton::leaveEvent(event);
    if (!isChecked()) {
        setIconSelected(false);
    }
}

void MenuButton::mousePressEvent(QMouseEvent* event)
{
    QPushButton::mousePressEvent(event);
    if (!isEnabled()) {
        return;
    }
    auto* ripple = new QtMaterialRipple(rect().center(), m_overlay, this);
    ripple->setRadiusEndValue(rect().width() / 2);
    ripple->setColor(theme::iconHighLightColor());
    if (isChecked()) {
        ripple->setOpacityStartValue(1);
    }
    m_overlay->addRipple(ripple);
}

void MenuButton::mouseDoubleClickEvent(QMouseEvent* event)
{
    QPushButton::mouseDoubleClickEvent(event);
    emit doubleClicked();
}

void MenuButton::paintEvent(QPaintEvent* e)
{
    QPushButton::paintEvent(e);
    if (m_value == 0) {
        return;
    }
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const auto r = rect();

    constexpr qreal penSize{3};

    auto f = font();
    f.setPixelSize(9);
    f.setBold(true);
    f.setWeight(QFont::ExtraBold);
    painter.setFont(f);
    const qreal miniSize = painter.fontMetrics().size(0, maxValue).width() + penSize + 1;

    const QRectF rc{r.topRight() - QPointF{miniSize, 0}, QSizeF{miniSize, miniSize}};

    if (m_needDrawBadgeCircle) {
        painter.save();
        painter.setBrush(theme::errorColor());
        painter.setPen(QPen(theme::topMenuColor(), penSize));
        painter.drawEllipse(rc);
        painter.restore();
    }

    const QString str(valueToString());
    const auto textSize = painter.fontMetrics().size(0, str);
    QPainterPath path;
    if (m_needDrawBadgeCircle) {
        path.addText(rc.center() - QPoint(textSize.width() / 2, -textSize.height() / 4), f, str);
    } else {
        path.addText(r.topRight() - QPoint(textSize.width() + 5, -textSize.height()), f, str);
    }
    painter.strokePath(path, QPen(theme::topMenuColor(), 3));
    painter.fillPath(path, QBrush(Qt::white));
}

QString MenuButton::valueToString() const noexcept
{
    if (m_value > 99) {
        return maxValue;
    }
    return QString::number(m_value);
}
