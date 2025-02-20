#include "LCDNumber.h"
#include "LCDWidget.h"
#include <qstyleoption.h>
#include <QPainter>

LCDNumber::LCDNumber(QWidget* parent)
  : LCDWidget(parent)
  , m_range(0, 10000)
{
    connect(this, &LCDWidget::valueChanged, this, [this](auto v) {
        setNumber(v.toInt());
    });
}

void LCDNumber::setNumber(int number)
{
    int n = fixup(number);
    emit numberChanged(n);
    int digitsCount = symbolCount();
    for (int i = 0; i < digitsCount; ++i, n /= 10) {
        char d = static_cast<char>(n % 10);
        int dPos = digitsCount - i - 1;
        setSymbol(dPos, QChar{d + '0'});
    }
}

int LCDNumber::currentNumber() const noexcept
{
    return this->toString().toInt();
}

void LCDNumber::setRange(int minValue, int maxValue)
{
    if (minValue < maxValue) {
        m_range.first = minValue;
        m_range.second = maxValue;
        fixup(currentNumber());
    }
}

int LCDNumber::fixup(int number) const
{
    if (number < m_range.first) {
        number = m_range.first;
    } else if (number > m_range.second) {
        number = m_range.second;
    }
    return number;
}

void LCDNumber::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    LCDWidget::paintEvent(event);
}
