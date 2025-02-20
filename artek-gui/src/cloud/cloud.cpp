#include "cloud.h"
#include <qboxlayout.h>

#include <qcolor.h>
#include <qlabel.h>
#include <qlayoutitem.h>
#include <qobject.h>
#include <qpainter.h>
#include <qpropertyanimation.h>
#include <qpushbutton.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qwidget.h>
#include "src/button/expand-button.h"
#include "src/gsl/pointers"

namespace {
class RotatingLabel : public QLabel
{
public:
    explicit RotatingLabel(QWidget* p)
      : QLabel(p)
    {}

    void rotate()
    {
        if (m_angle == 0) {
            constexpr int rotationAngle = 270;
            m_angle = rotationAngle;
        } else {
            m_angle = 0;
        }
        updateGeometry();
    }

    QSize minimumSizeHint() const override
    {
        auto s = QLabel::minimumSizeHint();
        if (m_angle == 0) {
            return s;
        }
        return s.transposed();
    }

    QSize sizeHint() const override
    {
        auto s = QLabel::sizeHint();
        if (m_angle == 0) {
            return s;
        }
        return s.transposed();
    }

    void paintEvent(QPaintEvent* e) override
    {
        if (m_angle != 0) {
            QPainter painter(this);
            painter.translate(rect().bottomLeft());
            painter.rotate(m_angle);
            painter.drawText(QRect(QPoint(0, 0), QLabel::sizeHint()), Qt::AlignCenter, text());
        } else {
            QLabel::paintEvent(e);
        }
    }

private:
    int m_angle{0};
};

gsl::owner<QLabel*> makeTitle(const QString& l, QWidget* p)
{
    gsl::owner<QLabel*> titleLb = new RotatingLabel(p);
    titleLb->setText(l);

    auto f = p->font();
    f.setPixelSize(20);
    titleLb->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);
    titleLb->setMinimumHeight(40);
    titleLb->setFont(f);
    titleLb->setAlignment(Qt::AlignCenter);
    return titleLb;
}

void swapLayoutItems(QHBoxLayout* hbFrom, QVBoxLayout* vb)
{
    QLayoutItem* item{};
    while ((item = hbFrom->takeAt(0)) != nullptr) {
        vb->insertItem(0, item);
        item->invalidate();
    }
}

void swapLayoutItems(QVBoxLayout* vbFrom, QHBoxLayout* hb)
{
    QLayoutItem* item{};
    while ((item = vbFrom->takeAt(0)) != nullptr) {
        hb->insertItem(0, item);
    }
}

}   // namespace

Cloud::Cloud(const QString& title, QWidget* p, const QColor& bgColor, QPair<qreal, qreal> collapseDegrees)
  : QWidget(p)
  , m_titleLb(makeTitle(title, this))
  , m_titleWidget(new QWidget(this))
  , m_hTitle(new QHBoxLayout(m_titleWidget))
  , m_contentWidget(new QWidget(this))
  , m_content(new QVBoxLayout(m_contentWidget))
{
    this->setAttribute(Qt::WA_StyledBackground);
    const bool needVerticalTitle =
      !(collapseDegrees == QPair<qreal, qreal>{0.0, 180.0} || collapseDegrees == QPair<qreal, qreal>{180.0, 0.0});
    if (needVerticalTitle) {
        m_vTitle = new QVBoxLayout;
        m_vTitle->setContentsMargins(0, 0, 0, 0);
    }
    m_expand = makeExpandButton(collapseDegrees, this);
    setObjectName("Cloud");
    setStyle(theme::foregroundColor(), Qt::PenStyle::NoPen, bgColor);

    m_content->setContentsMargins(0, 0, 0, 0);
    m_content->setSpacing(0);
    m_contentWidget->setVisible(false);
    m_contentWidget->setVisible(true);
    m_contentWidget->setObjectName("CloudContent");
    m_contentWidget->setStyleSheet(QString("background:%1;").arg(bgColor.name()));
    m_contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_expand->setCheckable(true);
    m_expand->setChecked(true);
    m_expand->setVisible(false);
    m_expand->setFixedSize(m_expand->minimumSizeHint());
    connect(m_expand, &QPushButton::toggled, this, &Cloud::setContentVisible);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_hTitle->addWidget(m_titleLb);
    m_hTitle->addWidget(m_expand);   //, 0, Qt::AlignTop | Qt::AlignRight);
    m_hTitle->setSpacing(6);
    m_hTitle->setContentsMargins(0, 0, 0, 0);
    m_titleWidget->setStyleSheet(m_contentWidget->styleSheet());

    gsl::owner<QVBoxLayout*> verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(0);
    verticalLayout->addWidget(m_titleWidget, 0, Qt::AlignTop);
    verticalLayout->addWidget(m_contentWidget);
}

Cloud::~Cloud()
{
    m_hTitle->deleteLater();
    if (m_vTitle != nullptr) {
        m_vTitle->deleteLater();
    }
}

QHBoxLayout* Cloud::titleLayout()
{
    return m_hTitle;
}

QVBoxLayout* Cloud::contentLayout()
{
    return m_content;
}

QString toString(Qt::PenStyle style)
{
    switch (style) {
    case Qt::SolidLine:
        return QLatin1String("solid");
    case Qt::DashLine:
        return QLatin1String("dashed");
    case Qt::DotLine:
        return QLatin1String("dotted");
    case Qt::DashDotLine:
        return QLatin1String("dot-dash");
    default:
        return QLatin1String("none");
    }
}

void Cloud::setExpandVisible(bool visible)
{
    m_expand->setVisible(visible);
}

void Cloud::setTitle(const QString& title)
{
    m_titleLb->setText(title);
}

void Cloud::setTitleVisible(bool visible)
{
    m_titleWidget->setVisible(visible);
}

void Cloud::setContentVisible(bool visible)
{
    if (contentIsVisible() == visible) {
        return;
    }
    m_contentWidget->setVisible(visible);
    m_expand->setChecked(visible);

    if (m_vTitle != nullptr) {
        dynamic_cast<RotatingLabel*>(m_titleLb)->rotate();
        if (visible) {
            swapLayoutItems(m_vTitle, m_hTitle);
            delete m_titleWidget->layout();
            m_vTitle = new QVBoxLayout;
            m_vTitle->setContentsMargins(0, 0, 0, 0);
            m_titleWidget->setLayout(m_hTitle);
            setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
            setMaximumWidth(4000);
        } else {
            swapLayoutItems(m_hTitle, m_vTitle);
            delete m_titleWidget->layout();
            m_hTitle = new QHBoxLayout;
            m_hTitle->setContentsMargins(0, 0, 0, 0);
            m_titleWidget->setLayout(m_vTitle);
            setMaximumWidth(52);
            setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        }
    } else {
        setSizePolicy(visible ? QSizePolicy::Preferred : QSizePolicy::Minimum,
                      visible ? QSizePolicy::Expanding : QSizePolicy::Minimum);
    }
    emit contentVisibilityChanged(visible);
}

bool Cloud::contentIsVisible() const noexcept
{
    return m_contentWidget->isVisibleTo(this);
}

QWidget* Cloud::contentWidget() const
{
    return m_contentWidget;
}

QString Cloud::titleText() const
{
    return m_titleLb->text();
}

void Cloud::bindCloud(Cloud* other)
{
    Q_ASSERT(this != other);
    this->setExpandVisible(true);
    other->setExpandVisible(true);
    connect(other, &Cloud::contentVisibilityChanged, this, [this](bool otherIsVisible) {
        if (otherIsVisible) {
            return;
        }
        setContentVisible(true);
    });
    connect(this, &Cloud::contentVisibilityChanged, other, [other](bool v) {
        if (v) {
            return;
        }
        other->setContentVisible(true);
    });
}

void Cloud::setStyle(QColor border, Qt::PenStyle style, QColor bgColor)
{
    setStyleSheet(QString("QWidget#Cloud{background:%1; border:3px %2 %3; border-radius: 15px;}QLabel{color:%4; "
                          "background:transparent;}")
                    .arg(bgColor.name(),    //
                         toString(style),   //
                         border.name(),     //
                         theme::textColor().name()));

    m_contentWidget->setStyleSheet(QString("background:%1;").arg(bgColor.name()));
    m_titleWidget->setStyleSheet(m_contentWidget->styleSheet());
}
