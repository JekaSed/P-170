#include <algorithm>
#include <cmath>
#include <qbrush.h>
#include <qline.h>
#include <qnamespace.h>
#include <qpen.h>
#include <utility>

#include <QPainter>
#include <QPolygon>
#include <QTransform>
#include <QColor>
#include <QEvent>
#include <QMouseEvent>
#include <QChar>
#include "LCDWidget.h"

// #include "utils.h"

namespace {
constexpr qreal SymbolWidth = 132;
constexpr qreal SymbolLeft = 15;
constexpr qreal SymbolRight = SymbolLeft + SymbolWidth - 48 - 1;
constexpr qreal SymbolHeight = 170;
constexpr qreal SymbolTop = 0;
constexpr qreal SymbolBottom = SymbolTop + SymbolHeight - 2;

const QPolygonF SegmentA = QPolygonF() << QPointF(SymbolLeft + 2, SymbolTop) << QPointF(SymbolRight - 2, SymbolTop)
                                       << QPointF(SymbolRight - 2 - 16, SymbolTop + 14)
                                       << QPointF(SymbolLeft + 2 + 16, SymbolTop + 14);
const QPolygonF SegmantB =
  QPolygonF() << QPointF(SymbolRight, SymbolTop + 2) << QPointF(SymbolRight, SymbolHeight / 2 - 2)
              << QPointF(SymbolRight - 14, SymbolHeight / 2 - 2 - 16) << QPointF(SymbolRight - 14, SymbolTop + 2 + 16);
const QPolygonF SegmantC = QPolygonF() << QPointF(SymbolRight, SymbolHeight / 2 + 2)
                                       << QPointF(SymbolRight, SymbolBottom - 2)
                                       << QPointF(SymbolRight - 14, SymbolBottom - 2 - 16)
                                       << QPointF(SymbolRight - 14, SymbolHeight / 2 + 2 + 16);
const QPolygonF SegmantD =
  QPolygonF() << QPointF(SymbolRight - 2, SymbolBottom) << QPointF(SymbolLeft + 2, SymbolBottom)
              << QPointF(SymbolLeft + 2 + 16, SymbolBottom - 14) << QPointF(SymbolRight - 2 - 16, SymbolBottom - 14);
const QPolygonF SegmantE =
  QPolygonF() << QPointF(SymbolLeft, SymbolBottom - 2) << QPointF(SymbolLeft, SymbolHeight / 2 + 2)
              << QPointF(SymbolLeft + 14, SymbolHeight / 2 + 2 + 16) << QPointF(SymbolLeft + 14, SymbolBottom - 2 - 16);
const QPolygonF SegmantF =
  QPolygonF() << QPointF(SymbolLeft, SymbolHeight / 2 - 2) << QPointF(SymbolLeft, SymbolTop + 2)
              << QPointF(SymbolLeft + 14, SymbolTop + 2 + 16) << QPointF(SymbolLeft + 14, SymbolHeight / 2 - 2 - 16);
const QPolygonF SegmantG =
  QPolygonF() << QPointF(SymbolLeft + 2, SymbolHeight / 2) << QPointF(SymbolLeft + 2 + 16, SymbolHeight / 2 - 9)
              << QPointF(SymbolRight - 2 - 16, SymbolHeight / 2 - 9) << QPointF(SymbolRight - 2, SymbolHeight / 2)
              << QPointF(SymbolRight - 2 - 16, SymbolHeight / 2 + 9)
              << QPointF(SymbolLeft + 2 + 16, SymbolHeight / 2 + 9);

const QList<const QPolygonF*> Segments = QList<const QPolygonF*>() << &SegmentA << &SegmantB << &SegmantC << &SegmantD
                                                                   << &SegmantE << &SegmantF << &SegmantG;
constexpr int SegmentsCount = 7;

constexpr int segmentsOffsetY{6};

// clang-format off
const bool DigitSymbols[10][SegmentsCount] =
{
    {true,  true,  true,  true,  true,  true,  false},
    {false, true,  true,  false, false, false, false},
    {true,  true,  false, true,  true,  false, true},
    {true,  true,  true,  true,  false, false, true},
    {false, true,  true,  false, false, true,  true},
    {true,  false, true,  true,  false, true,  true},
    {true,  false, true,  true,  true,  true,  true},
    {true,  true,  true,  false, false, false, false},
    {true,  true,  true,  true,  true,  true,  true},
    {true,  true,  true,  true,  false, true,  true},
};
// clang-format on

const bool OffSymbol[SegmentsCount] = {false, false, false, false, false, false, false};

const qreal PointRadius = 20;
const QPointF PointCoord = QPointF(SymbolRight + 2 * PointRadius + 7, SymbolBottom - PointRadius / 2);

const QVector<QPointF> ColonCoord(
  {QPointF(SymbolRight + 4 * PointRadius / 2, SymbolBottom - SymbolHeight / 2 + SymbolHeight / 5),

   QPointF(SymbolRight + 4 * PointRadius / 2, SymbolBottom - SymbolHeight / 2 - SymbolHeight / 5)});

struct Symbol
{
    Symbol()
      : on{false}
      , isEnabledPoint{false}
      , pointOn{false}
      , hovered(false)
      , pos(0)
      , type(LCDWidget::Type::POINT)
    {}

    QChar ch;
    bool on;
    bool isEnabledPoint;
    bool pointOn;
    bool hovered;
    int pos;
    LCDWidget::Type type;
};

using Symbols = std::vector<Symbol>;
}   // namespace

class LCDWidget::LCDWidgetPrivate
{
public:
    explicit LCDWidgetPrivate(LCDWidget* q)
      : m_q{q}
      , m_prev(nullptr)
      , m_next(nullptr)
      , m_onColor{Qt::black}
      , m_heightHint{20}
      , m_enabled(true)
      , m_hasDrawOffSegments{false}
      , m_interactive(false)
      , m_relationship(true)
    {}

    void paintSymbols(QPainter& p) const
    {
        const QSize sSize = calcSymbolSize();

        QRect sRect(this->m_q->contentsMargins().left(), segmentsOffsetY, sSize.width(),
                    sSize.height() - segmentsOffsetY * 2);
        const qreal k = sRect.height() / SymbolHeight;
        const int symbolsCount = symbols.size();
        int leadingZeroesCount{0};
        if (!m_showLeadingZeroes) {
            while (symbolsCount >= 1) {
                const bool isZero = symbols[leadingZeroesCount].ch == QChar('0');
                if (!isZero) {
                    break;
                }
                leadingZeroesCount++;
            }
        }
        for (int i = 0; i < symbolsCount; ++i) {
            QTransform transform = QTransform::fromTranslate(sRect.left(), sRect.top());
            transform.scale(k, k);
            if (i >= leadingZeroesCount) {
                paintSymbol(p, symbols[i], transform);
            }
            sRect.translate(sSize.width(), 0);
        }
    }

    void setHovered(const QPoint& pos)
    {
        QSize sSize = calcSymbolSize();
        int hoveredPos = pos.x() / sSize.width();
        QRect sRect(0, 0, sSize.width(), sSize.height());
        sRect.translate(hoveredPos * sSize.width(), 0);
        if (sRect.contains(pos)) {
            for (int i = 0; i < symbols.size(); ++i) {
                symbols[i].hovered = i == hoveredPos;
            }
        } else {
            clearHovered();
        }
    }

    void clearHovered()
    {
        auto* s = hoveredSymbol();
        if (s == nullptr) {
            return;
        }
        s->hovered = false;
    }

    void incrementDigit(int pos)
    {
        if (!m_q->m_d->m_enabled) {
            return;
        }
        auto& s = symbols[pos];
        if (!s.ch.isDigit()) {
            return;
        }
        const int currentValue = s.ch.digitValue();
        const bool canDoInc = currentValue < 9;

        if (canDoInc) {
            s.ch = QChar('0' + currentValue + 1);
            emit m_q->valueChanged(m_q->toString());
        } else {
            if (s.pos > 0 && !allIsChar(s.pos, QChar('9'))) {
                s.ch = QChar('0');
                incrementDigit(s.pos - 1);
                return;
            }
            if (m_prev != nullptr && m_prev->isEnabled() && m_relationship) {
                m_prev->m_d->incrementDigit(m_prev->symbolCount() - 1);
                m_prev->update();
            }
        }
    }

    bool allIsChar(int pos, const QChar ch)
    {
        bool res{true};
        for (int i = 0; i < pos; ++i) {
            res &= symbols[i].ch == ch;
        }
        return res;
    }

    void decrementDigit(int pos)
    {
        if (!m_q->m_d->m_enabled) {
            return;
        }
        auto& s = symbols[pos];
        if (!s.ch.isDigit()) {
            return;
        }
        const int currentDigit = s.ch.digitValue();
        const bool canDoDec = currentDigit > 0;
        if (canDoDec) {
            s.ch = QChar('0' + currentDigit - 1);
            emit m_q->valueChanged(m_q->toString());

        } else {
            if (s.pos > 0 && !allIsChar(s.pos, QChar('0'))) {
                s.ch = QChar('9');
                decrementDigit(s.pos - 1);
                return;
            }
            if (m_prev != nullptr && m_prev->isEnabled() && m_relationship) {
                m_prev->m_d->decrementDigit(m_prev->symbolCount() - 1);
                m_prev->update();
            }
        }
    }

    void changeHover(bool moveToUp)
    {
        auto* s = hoveredSymbol();
        if (s == nullptr) {
            return;
        }

        if (moveToUp) {
            incrementDigit(s->pos);
        } else {
            decrementDigit(s->pos);
        }
    }

    void moveHover(bool moveToLeft)
    {
        auto* s = hoveredSymbol();
        if (s == nullptr) {
            return;
        }
        const int count = symbols.size();
        if (moveToLeft) {
            if (s->pos > 0) {
                s->hovered = false;
                symbols[s->pos - 1].hovered = true;
            } else if (m_prev != nullptr) {
                clearHovered();
                m_prev->m_d->setHovered(m_prev->symbolCount() - 1);
            }
        } else {
            if (s->pos < count - 1) {
                s->hovered = false;
                symbols[s->pos + 1].hovered = true;
            } else if (m_next != nullptr) {
                clearHovered();
                m_next->m_d->setHovered(0);
            }
        }
    }

    Symbol* hoveredSymbol()
    {
        auto it = std::find_if(symbols.begin(), symbols.end(), [](const Symbol& symbol) {
            return symbol.hovered;
        });
        if (it != symbols.end()) {
            return &symbols[std::distance(symbols.begin(), it)];
        }
        return nullptr;
    }

    QString toString() const noexcept
    {
        QString res;
        const int count = symbols.size();
        res.resize(count);
        for (int i = 0; i < count; ++i) {
            res[i] = symbols[i].ch;
        }
        return res;
    }

    void attach(LCDWidget* rightLcd)
    {
        m_next = rightLcd;
        rightLcd->m_d->m_prev = m_q;
    }

    void setHovered(int pos)
    {
        auto* s = hoveredSymbol();
        if (s != nullptr) {
            s->hovered = false;
        }
        m_q->activateWindow();
        symbols[pos].hovered = true;
        m_q->setFocus();
        m_q->update();
    }

    void changeHoverNumber(const QChar enteredChar)
    {
        auto* s = hoveredSymbol();
        if (s == nullptr) {
            return;
        }
        QString str;
        if (s->ch != enteredChar) {
            s->ch = enteredChar;
            str = toString();
        }
        if (s->pos < symbols.size() - 1) {
            s->hovered = false;
            symbols[s->pos + 1].hovered = true;
        } else if (m_next != nullptr) {
            s->hovered = false;
            m_next->m_d->setHovered(0);
        }

        if (!str.isEmpty()) {
            emit m_q->valueChanged(str);
        }
        m_q->update();
    }

    void paintSymbol(QPainter& p, const Symbol& symbol, const QTransform& transform) const
    {
        QBrush offColor = Qt::white;

        if (symbol.on) {
            if (symbol.ch.isDigit()) {
                const bool* flags = DigitSymbols[symbol.ch.toLatin1() - '0'];
                paintSegments(p, flags, transform, m_onColor, offColor, symbol.hovered);
            } else if (symbol.ch.isNull()) {
                paintSegments(p, OffSymbol, transform, m_onColor, offColor, symbol.hovered);
            }
        } else {
            paintSegments(p, OffSymbol, transform, m_onColor, offColor, symbol.hovered);
        }

        if (symbol.isEnabledPoint) {
            p.setBrush(symbol.pointOn ? m_onColor : offColor);
            qreal pr = transform.m11() * PointRadius;
            if (symbol.type == Type::POINT) {
                QPointF pc = transform.map(PointCoord);
                p.drawEllipse(pc, pr, pr);
            } else if (symbol.type == Type::COLON) {
                for (QPointF point : ColonCoord) {
                    QPointF pc = transform.map(point);
                    p.drawEllipse(pc, pr, pr);
                }
            }
        }
    }

    void paintSegments(QPainter& p, const bool* flags, const QTransform& transform, const QBrush& onBrush, const QBrush,
                       bool useLighter = false) const
    {
        auto b = onBrush;
        if (useLighter) {
            if (b.color() == Qt::black || b.color() == Qt::white) {
                b.setColor(Qt::gray);
            } else {
                b.setColor(b.color().lighter());
            }
            p.setPen(QPen(onBrush.color(), 2));
            auto botLine = transform.map(QLineF(SymbolLeft, SymbolBottom, SymbolRight, SymbolBottom));
            auto p1 = botLine.p1();
            p1.ry() += segmentsOffsetY / 2;
            auto p2 = botLine.p2();
            p2.ry() += segmentsOffsetY / 2;
            p.drawLine(p1, p2);
        }
        p.setPen(b.color());
        p.setBrush(b);
        for (int i = 0; i < SegmentsCount; ++i) {
            if (flags[i]) {
                p.drawPolygon(transform.map(*Segments[i]));
            }
        }
        p.setPen(Qt::NoPen);

        //  echo symbols
        if (m_hasDrawOffSegments) {
            const QPen defPen(QColor(33, 33, 33, 127));

            p.setPen(defPen);
            p.setBrush(Qt::gray);
            for (int i = 0; i < SegmentsCount; ++i) {
                if (!flags[i]) {
                    p.drawPolygon(transform.map(*Segments[i]));
                }
            }
            p.setPen(Qt::NoPen);
        }
    }

    QSize calcSymbolSize() const
    {
        int symbolsCount = symbols.size();
        if (symbolsCount == 0) {
            return {};
        }

        double origSymbolsWidth = symbolsCount * SymbolWidth;

        QRect wRect = m_q->rect();
        qreal k = wRect.height() / static_cast<double>(SymbolHeight);
        if (static_cast<int>(k * origSymbolsWidth) > wRect.width()) {
            k = wRect.width() / origSymbolsWidth;
        }

        return {static_cast<int>(k * SymbolWidth), static_cast<int>(k * SymbolHeight)};
    }

    LCDWidget* m_q;
    LCDWidget* m_prev;
    LCDWidget* m_next;
    Symbols symbols;
    QBrush m_onColor;
    int m_heightHint;
    bool m_enabled;
    bool m_hasDrawOffSegments;
    bool m_interactive;
    bool m_relationship;
    bool m_showLeadingZeroes{true};
};

const QChar LCDWidget::InvalidChar = QChar();

LCDWidget::LCDWidget(QWidget* parent)
  : QWidget(parent)
  , m_d{new LCDWidgetPrivate(this)}
{
    setAttribute(Qt::WA_Hover);
}

LCDWidget::~LCDWidget()
{
    delete m_d;
}

void LCDWidget::setSymbolCount(int count)
{
    m_d->symbols.resize(count);
    for (int i = 0; i < count; ++i) {
        Symbol s{};
        s.pos = i;
        m_d->symbols[i] = s;
    }
    updateGeometry();
}

int LCDWidget::symbolCount() const
{
    return m_d->symbols.size();
}

void LCDWidget::setSymbol(int pos, QChar sym, bool on, Type type)
{
    Symbol& s = m_d->symbols[pos];
    s.ch = sym;
    s.on = on;
    s.type = type;
    update();
}

QChar LCDWidget::symbol(int pos, bool* on) const
{
    const Symbol& s = m_d->symbols[pos];
    if (on != nullptr) {
        *on = s.on;
    }
    return s.ch;
}

void LCDWidget::enablePoint(int pos, bool enable, bool on, Type type)
{
    Symbol& symbol = m_d->symbols[pos];
    symbol.isEnabledPoint = enable;
    symbol.pointOn = on;
    symbol.type = type;
}

bool LCDWidget::isEnabledPoint(int pos, Type& type, bool* on) const
{
    const Symbol& symbol = m_d->symbols[pos];
    if (on != nullptr) {
        *on = symbol.pointOn;
        type = symbol.type;
    }
    return symbol.isEnabledPoint;
}

QRect LCDWidget::symbolRect(int pos) const
{
    QSize sSize = m_d->calcSymbolSize();
    QRect sRect(0, 0, sSize.width(), sSize.height());
    sRect.translate(pos * sSize.width(), 0);
    return sRect;
}

void LCDWidget::setDrawOffSegments(bool enable)
{
    m_d->m_hasDrawOffSegments = enable;
    update();
}

bool LCDWidget::hasDrawOffSegments() const
{
    return m_d->m_hasDrawOffSegments;
}

void LCDWidget::setHeightHint(int hh)
{
    m_d->m_heightHint = hh;
    adjustSize();
}

int LCDWidget::widthForHeight(int h) const
{
    qreal k = qreal(h) / SymbolHeight;
    qreal w = k * SymbolWidth * symbolCount();
    return static_cast<int>(w);
}

void LCDWidget::setOnColor(const QBrush& color)
{
    m_d->m_onColor = color;
    this->update();
}

void LCDWidget::setEnabled(bool enabled)
{
    m_d->m_enabled = enabled;
}

QString LCDWidget::toString() const noexcept
{
    return m_d->toString();
}

void LCDWidget::attachRightLcd(LCDWidget* lcd)
{
    m_d->attach(lcd);
}

void LCDWidget::setInteractive(bool enable)
{
    m_d->m_interactive = enable;
}

void LCDWidget::setRelationship(bool enable)
{
    m_d->m_relationship = enable;
}

void LCDWidget::setShowLeadingZeroes(bool show)
{
    m_d->m_showLeadingZeroes = show;
    update();
}

void LCDWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    m_d->paintSymbols(p);
}

void LCDWidget::wheelEvent(QWheelEvent* event)
{
    m_d->changeHover(event->angleDelta().y() >= 0);
    update();
}

void LCDWidget::mousePressEvent(QMouseEvent* event)
{
    m_d->changeHover(event->button() == Qt::RightButton);
    update();
}

bool LCDWidget::processKeyEvent(QKeyEvent* event)
{
    const int key = event->key();
    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        m_d->changeHoverNumber(QChar('0' + (key - Qt::Key_0)));
        event->accept();
        return true;
    }
    if (key == Qt::Key_Left || key == Qt::Key_Right) {
        m_d->moveHover(key == Qt::Key_Left);
        this->update();
        event->accept();
        return true;
    }
    if (key == Qt::Key_Up || key == Qt::Key_Down) {
        m_d->changeHover(key == Qt::Key_Up);
        this->update();
        event->accept();
        return true;
    }
    return false;
}

bool LCDWidget::event(QEvent* event)
{
    if (!m_d->m_interactive || !isEnabled()) {
        return QWidget::event(event);
    }
    switch (event->type()) {
    case QEvent::HoverEnter:
    case QEvent::HoverMove: {
        auto* e = dynamic_cast<QHoverEvent*>(event);
        if (e != nullptr) {
            m_d->setHovered(e->position().toPoint());
            this->setFocus();
            this->update();
            event->accept();
            return true;
        }
        break;
    }
    case QEvent::HoverLeave:
        m_d->clearHovered();
        this->update();
        break;
    case QEvent::KeyPress:
        return processKeyEvent(static_cast<QKeyEvent*>(event));
    default:
        break;
    }
    return QWidget::event(event);
}

QSize LCDWidget::sizeHint() const
{
    return {widthForHeight(m_d->m_heightHint), m_d->m_heightHint};
}

QSize LCDWidget::minimumSizeHint() const
{
    return sizeHint();
}
