#pragma once

#include "LCDWidget.h"

class LCDNumber : public LCDWidget
{
    Q_OBJECT
public:
    explicit LCDNumber(QWidget* parent = nullptr);
    void setNumber(int number);
    int currentNumber() const noexcept;
    void setRange(int minValue, int maxValue);

signals:
    void numberChanged(int value);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int fixup(int number) const;

private:
    QPair<int, int> m_range;
};
