#include "expand-button.h"
#include "src/theme/theme.h"
#include <qabstractbutton.h>
#include <qcolor.h>
#include <qeasingcurve.h>
#include <qglobal.h>
#include <qline.h>
#include <qlist.h>
#include <QDebug>
#include <QToolTip>
#include <qpainter.h>
#include <qparallelanimationgroup.h>
#include <qpen.h>
#include <qpoint.h>
#include <qpropertyanimation.h>
#include <qsequentialanimationgroup.h>
#include <qsize.h>
#include <qwidget.h>

namespace {
constexpr qreal widthToHeight = 307.0 / 172;

class ExpandButton final : public QAbstractButton
{
    Q_OBJECT
    Q_PROPERTY(qreal angle READ angle WRITE setAngle)
    Q_PROPERTY(QColor color READ color WRITE setColor)
public:
    explicit ExpandButton(QPair<qreal, qreal> degrees, QWidget* p)
      : QAbstractButton(p)
      , m_angle(degrees.first)
    {
        setCheckable(true);
        setCursor(Qt::PointingHandCursor);

        auto* angleAnimation = new QPropertyAnimation(this, "angle", this);
        auto* colorAnimation = new QPropertyAnimation(this, "color", this);

        constexpr int duration{350};
        colorAnimation->setDuration(duration * 4);
        colorAnimation->setEasingCurve(QEasingCurve::InOutBack);
        angleAnimation->setDuration(duration);
        angleAnimation->setEasingCurve(QEasingCurve::InOutBack);

        angleAnimation->setStartValue(degrees.first);
        angleAnimation->setEndValue(degrees.second);

        colorAnimation->setStartValue(theme::iconColor());
        colorAnimation->setEndValue(theme::iconHighLightColor());

        connect(this, &QAbstractButton::toggled, this, [angleAnimation, colorAnimation](bool toggle) {
            const auto direction =
              toggle ? QPropertyAnimation::Direction::Backward : QPropertyAnimation::Direction::Forward;
            angleAnimation->setDirection(direction);
            colorAnimation->setDirection(direction);
            angleAnimation->start();
            colorAnimation->start();
        });
    }

    QColor color() const noexcept
    {
        return m_color;
    }

    void setColor(QColor newColor)
    {
        m_color = newColor;
        update();
    }

    qreal angle() const noexcept
    {
        return m_angle;
    }

    void setAngle(qreal angle)
    {
        m_angle = angle;
        update();
    }

    QSize sizeHint() const noexcept override
    {
        auto s = size();
        return QSize(qreal(s.height()) * widthToHeight, s.height());
    }

    QSize minimumSizeHint() const noexcept override
    {
        return {35, 20};
    }

    void enterEvent(QEnterEvent* event) override
    {
        QAbstractButton::enterEvent(event);
        m_color = theme::iconHighLightColor();
        update();
        const auto r = rect();
        const auto toolTipText = isChecked() ? tr("Свернуть") : tr("Развернуть");
        QToolTip::showText(mapToGlobal(r.bottomLeft()), toolTipText, this, r);
    }

    void leaveEvent(QEvent* event) override
    {
        m_color = isChecked() ? theme::iconColor() : theme::iconHighLightColor();
        update();
        QAbstractButton::leaveEvent(event);
    }

protected:
    void paintEvent(QPaintEvent* /*e*/) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        constexpr int penWidth{3};
        auto r = rect();
        p.setPen(QPen(m_color, penWidth, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap, Qt::RoundJoin));
        const auto center = r.center();
        r.setSize(QSize(r.width() - 2 * penWidth, r.height() - 2 * penWidth));
        r.moveCenter(center);

        const auto p1 = r.bottomLeft();
        const auto p2 = QPointF(center.x(), r.top());
        const auto p3 = r.bottomRight();

        QLineF l1(p1, p2);
        QLineF l2(p2, p3);
        QList<QLineF> lines;
        lines.append(l1);
        lines.append(l2);

        const auto smallL1 = QLineF(QPointF(p2.x(), p2.y() + penWidth * 2), QPointF(p1.x() + penWidth * 2, p1.y()));
        const auto smallL2 = QLineF(smallL1.p1(), QPointF(p3.x() - penWidth * 2, p3.y()));

        QList<QLineF> smallLines;
        smallLines.append(smallL1);
        smallLines.append(smallL2);

        p.translate(width() / 2, height() / 2);
        p.rotate(m_angle);
        p.translate(-width() / 2, -height() / 2);
        p.drawLines(lines);

        auto thinPen = p.pen();
        thinPen.setWidth(penWidth - 1);
        p.setPen(thinPen);
        p.drawLines(smallLines);
    }

private:
    qreal m_angle{};
    QColor m_color{theme::iconColor()};
};

}   // namespace

QAbstractButton* makeExpandButton(QPair<qreal, qreal> degrees, QWidget* parent)
{
    return new ExpandButton(degrees, parent);
}

#include "expand-button.moc"
