#include <algorithm>
#include <functional>

#include <QAction>
#include <QHBoxLayout>
#include <QMenu>
#include <QMoveEvent>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QEvent>
#include <QPoint>
#include <QStyle>
#include <QTimer>
#include <qwidget.h>
#include <utility>
#include <qbrush.h>
#include <qcolor.h>
#include <qdebug.h>
#include <qglobal.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qsizepolicy.h>
#include <qstyleoption.h>

#include "LCDWidget.h"
#include "FreqEditWidget.h"

namespace {

QPoint fromEvent(QMouseEvent* event)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return event->pos();
#else
    return event->position().toPoint();
#endif
}

QPoint fromEvent(QWheelEvent* event)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    return event->pos();
#else
    return event->position().toPoint();
#endif
}

class DigitWidget : public LCDWidget
{
public:
    using DigitWidgetDelegate = std::function<void(QWidget*, int)>;

public:
    explicit DigitWidget(DigitWidgetDelegate inc, DigitWidgetDelegate dec, QWidget* parent = nullptr)
      : LCDWidget{parent}
      , m_incDigitDelegate(std::move(inc))
      , m_decDigitDelegate(std::move(dec))
    {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

protected:   // QWidget
    void wheelEvent(QWheelEvent* event) override
    {
        int sn = symbolNumber(fromEvent(event));
        if (sn != -1) {
            if (event->angleDelta().y() < 0) {
                m_decDigitDelegate(this, sn);
                event->accept();
                return;
            }
            m_incDigitDelegate(this, sn);
            event->accept();
            return;
        }
        LCDWidget::wheelEvent(event);
    }

    void mousePressEvent(QMouseEvent* event) override
    {
        int sn = symbolNumber(fromEvent(event));
        if (sn != -1) {
            if (event->button() == Qt::RightButton) {
                m_incDigitDelegate(this, sn);

                event->accept();
                return;
            }
            m_decDigitDelegate(this, sn);
            event->accept();
            return;
        }
        LCDWidget::mousePressEvent(event);
    }

private:
    int symbolNumber(const QPoint& p) const
    {
        int count = symbolCount();
        for (int i = 0; i < count; ++i) {
            if (symbolRect(i).contains(p)) {
                return i;
            }
        }
        return -1;
    }

private:
    DigitWidgetDelegate m_incDigitDelegate;
    DigitWidgetDelegate m_decDigitDelegate;
};

class PresetFreqsWidget : public QWidget
{
public:
    static constexpr int ButtonSize = 20;

public:
    explicit PresetFreqsWidget(QWidget* parent = nullptr)
      : QWidget{parent}
      , predPresetFreqToolBtn{new QToolButton(this)}
      , showPresetFreqToolBtn{new QToolButton(this)}
      , nextPresetFreqToolBtn{new QToolButton(this)}
    {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        QHBoxLayout* l = new QHBoxLayout(this);
        l->setContentsMargins(0, 0, 0, 1);
        l->setSpacing(0);

        predPresetFreqToolBtn->setArrowType(Qt::LeftArrow);
        predPresetFreqToolBtn->setFixedSize(ButtonSize, ButtonSize);
        l->addWidget(predPresetFreqToolBtn);

        showPresetFreqToolBtn->setText("ЗПЧ");
        showPresetFreqToolBtn->setPopupMode(QToolButton::InstantPopup);
        //showPresetFreqToolBtn->setFixedSize(ButtonSize, ButtonSize);
        showPresetFreqToolBtn->setFixedHeight(ButtonSize);
        l->addWidget(showPresetFreqToolBtn);

        nextPresetFreqToolBtn->setArrowType(Qt::RightArrow);
        nextPresetFreqToolBtn->setFixedSize(ButtonSize, ButtonSize);
        l->addWidget(nextPresetFreqToolBtn);

        updateSize();
    }

    void updateSize()
    {
        setFixedSize(sizeHint());
        updateGeometry();
    }

public:   // QWidget
    void changeEvent(QEvent* event) override
    {
        QWidget::changeEvent(event);
        updateSize();
    }

public:
    QToolButton* predPresetFreqToolBtn;
    QToolButton* showPresetFreqToolBtn;
    QToolButton* nextPresetFreqToolBtn;
};
}   // namespace

class FreqEditWidget::FreqEditWidgetPrivate
{
public:
    explicit FreqEditWidgetPrivate(FreqEditWidget* q)
      : freqEditWidget{q}
      , presetFreqWidget(new PresetFreqsWidget(q))
      , currPresetFreqNum{-1}
      , minorDigitWidget{createDigitWidget(3, false)}
      , majorDigitWidget{createDigitWidget(0, false)}
      , minFreq{FreqEditWidget::DefaultMinFreq}
      , maxFreq{FreqEditWidget::DefaultMaxFreq}
      , freq{FreqEditWidget::DefaultFreq}
    {
        // q->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        QHBoxLayout* mainHBoxLayout = new QHBoxLayout(q);
        mainHBoxLayout->setContentsMargins(0, 0, 0, 0);
        mainHBoxLayout->setSpacing(0);

        mainHBoxLayout->setAlignment(Qt::AlignLeft);
        mainHBoxLayout->addWidget(majorDigitWidget, 0, Qt::AlignLeft | Qt::AlignBottom);

        m_lastModified.setSingleShot(true);

        auto* pfVBoxLayout = new QVBoxLayout;
        pfVBoxLayout->setContentsMargins(0, 0, 0, 0);
        pfVBoxLayout->setSpacing(0);

        presetFreqWidget->setHidden(true);
        q->connect(presetFreqWidget->nextPresetFreqToolBtn, SIGNAL(clicked()), SLOT(onNextPresetFreq()));
        q->connect(presetFreqWidget->predPresetFreqToolBtn, SIGNAL(clicked()), SLOT(onPredPresetFreq()));
        pfVBoxLayout->addWidget(presetFreqWidget, 0, Qt::AlignLeft | Qt::AlignBottom);

        pfVBoxLayout->addWidget(minorDigitWidget, 0, Qt::AlignLeft | Qt::AlignBottom);
        mainHBoxLayout->addLayout(pfVBoxLayout);

        majorDigitWidget->attachRightLcd(minorDigitWidget);

        majorDigitWidget->setInteractive(true);
        minorDigitWidget->setInteractive(true);

        connect(majorDigitWidget, SIGNAL(valueChanged(QString)), q, SLOT(onDigitChanged(QString)));
        connect(minorDigitWidget, SIGNAL(valueChanged(QString)), q, SLOT(onDigitChanged(QString)));
    }

    void calcAndSetFreq()
    {
        bool convertOk{};
        long newFreq = QString(majorDigitWidget->toString() + minorDigitWidget->toString()).toLong(&convertOk);
        if (convertOk) {
            setFreq(newFreq, false);
        }
    }

    void update()
    {
        if (minFreq > maxFreq) {
            std::swap(minFreq, maxFreq);
        }

        if (freq < minFreq) {
            freq = minFreq;
        } else if (freq > maxFreq) {
            freq = maxFreq;
        }

        //блокирует вперед и назад если зпч заблокированы
        bool enable{false};
        if (presetFreqWidget->showPresetFreqToolBtn->menu() != nullptr) {
            for (auto* a : presetFreqWidget->showPresetFreqToolBtn->menu()->actions()) {
                if (a->isEnabled()) {
                    enable = true;
                    break;
                }
            }
        }

        bool enablePredPresetFreq = currPresetFreqNum > 0;
        presetFreqWidget->predPresetFreqToolBtn->setEnabled(enablePredPresetFreq && enable);

        bool enableNextPresetFreq = (currPresetFreqNum + 1) < presetFreqs.size();
        presetFreqWidget->nextPresetFreqToolBtn->setEnabled(enableNextPresetFreq && enable);

        int n = static_cast<int>(freq);
        int minorDigitCount = minorDigitWidget->symbolCount();
        for (int i = 0; i < minorDigitCount; ++i, n /= 10) {
            char d = static_cast<char>(n % 10);
            int dPos = minorDigitCount - i - 1;
            minorDigitWidget->setSymbol(dPos, QChar{d + '0'});
        }

        int majorDigitCount = majorDigitWidget->symbolCount();
        for (int i = 0; i < majorDigitCount; ++i, n /= 10) {
            char d = static_cast<char>(n % 10);
            int dPos = majorDigitCount - i - 1;
            majorDigitWidget->setSymbol(dPos, QChar{d + '0'});
        }
    }

    void updateSize(const QSize parentSize) const
    {
        const auto maxWidth = parentSize.width();

        int heightForMajor = parentSize.height();
        while (true) {
            majorDigitWidget->setHeightHint(heightForMajor);
            int minorHeight = heightForMajor - presetFreqWidget->height();
            if (!presetFreqWidget->isVisible()) {
                minorHeight = heightForMajor * 0.75;
            }
            minorDigitWidget->setHeightHint(minorHeight);
            const auto preferredSize = majorDigitWidget->sizeHint().width() + minorDigitWidget->sizeHint().width();
            if (preferredSize <= maxWidth) {
                break;
            }
            --heightForMajor;
        }
        majorDigitWidget->setFixedSize(majorDigitWidget->sizeHint());
        minorDigitWidget->setFixedSize(minorDigitWidget->sizeHint());
        freqEditWidget->updateGeometry();
    }

    void updateDigitWidgets()
    {
        int digitCount = 1;
        for (int maxFreqVal = static_cast<int>(maxFreq); maxFreqVal > 9; maxFreqVal /= 10) {
            ++digitCount;
        }

        if (digitCount < minorDigitWidget->symbolCount()) {
            digitCount = minorDigitWidget->symbolCount();
        }

        int majorDigitCount = digitCount - minorDigitWidget->symbolCount();
        if (majorDigitCount < 0) {
            majorDigitCount = 0;
        }

        majorDigitWidget->setSymbolCount(majorDigitCount);
        for (int i = 0; i < majorDigitCount; ++i) {
            if ((i % 3) == 0) {
                int pos = majorDigitCount - i - 1;
                majorDigitWidget->enablePoint(pos);
            }
        }

        updateSize(freqEditWidget->size());
    }

    void updatePresetFreqsWidget()
    {
        if (!presetFreqs.empty()) {
            presetFreqWidget->setEnabled(true);

            auto* menu = new QMenu(presetFreqWidget);

            for (auto it = presetFreqs.begin(); it != presetFreqs.end(); ++it) {
                auto freqNum = it.key();
                bool const enable = it.value().first;

                QString const antenna = QString::fromStdString(it.value().second.second);
                QString text = QString::number(it.value().second.first);
                if (!antenna.isEmpty()) {
                    text += "   (" + antenna + ")";
                }

                QIcon icon;
                if (usedPassRange) {   //если используем интервал прохождения, проверяем на вхождение частоты
                    icon = presetFreqWidget->style()->standardIcon(
                      isPassing(it.value().first) ? QStyle::SP_DialogOkButton : QStyle::SP_MessageBoxWarning);
                }
                QAction* action = menu->addAction(icon, QString::number(freqNum + 1) + ":   " + text);
                action->setData(QVariant::fromValue(freqNum));
                action->setEnabled(enable);
                freqEditWidget->connect(action, SIGNAL(triggered()), SLOT(onSelectPresetFreq()));
                ++freqNum;
            }
            presetFreqWidget->showPresetFreqToolBtn->setMenu(menu);
        } else {
            presetFreqWidget->setEnabled(false);
        }

        update();
    }

    DigitWidget* createDigitWidget(int digitCount, bool showOffSegments)
    {
        auto* digitWidget = new DigitWidget(
          [this](QWidget* s, int symbolNumber) {
              freqEditWidget->onChangedDigit(s, symbolNumber, true);
          },
          [this](QWidget* s, int symbolNumber) {
              freqEditWidget->onChangedDigit(s, symbolNumber, false);
          },
          freqEditWidget);
        digitWidget->setSymbolCount(digitCount);
        digitWidget->setDrawOffSegments(showOffSegments);
        digitWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        return digitWidget;
    }

    void setFreqColor(QColor color) const
    {
        const QBrush brush(color);
        minorDigitWidget->setOnColor(brush);
        majorDigitWidget->setOnColor(brush);
    }

    void setFreq(long int newFreq, bool edited)
    {
        if (newFreq == freq) {
            return;
        }
        if (newFreq < minFreq) {
            setFreq(minFreq, edited);
            return;
        }
        if (newFreq > maxFreq) {
            setFreq(maxFreq, edited);
            return;
        }

        freq = newFreq;

        long int freqNum{0};

        for (Freqs::iterator it = presetFreqs.begin(); it != presetFreqs.end(); ++it) {
            if (freq == it.value().second.first) {
                freqNum = it.key();
            }
        }
        if (freqNum != -1) {
            currPresetFreqNum = freqNum;
        }
        update();

        if (edited) {
            emit freqEditWidget->freqEdited(freq);
        } else {
            m_lastModified.start(500);
            emit freqEditWidget->freqChanged(freq);
        }
    }

    void setForecastingRange(int mpch, int orch)
    {
        usedPassRange = true;
        m_mpch = mpch;
        m_orch = orch;
        updatePresetFreqsWidget();
    }

    bool isPassing(int freq) const
    {   //! возвращает признак прохождения частоты в заданном интервале
        return (m_mpch >= freq && freq >= 0.9 * m_orch);
    }

    FreqEditWidget* freqEditWidget;

    PresetFreqsWidget* presetFreqWidget;
    Freqs presetFreqs;
    long int currPresetFreqNum;

    DigitWidget* minorDigitWidget;
    DigitWidget* majorDigitWidget;

    long int minFreq;
    long int maxFreq;
    long int freq;

    QTimer m_lastModified;

    int m_mpch = 0;
    int m_orch = 0;
    bool usedPassRange = false;
};

const int FreqEditWidget::DefaultMinFreq = 0;
const int FreqEditWidget::DefaultMaxFreq = 99999999;
const int FreqEditWidget::DefaultFreq = 3240000;

FreqEditWidget::FreqEditWidget(QWidget* parent)
  : QWidget(parent)
  , m_d(new FreqEditWidgetPrivate(this))
{
    m_d->updateDigitWidgets();
    m_d->updatePresetFreqsWidget();
    setAttribute(Qt::WA_NoMousePropagation);
}

FreqEditWidget::~FreqEditWidget()
{
    delete m_d;
}

long int FreqEditWidget::maxFreq() const
{
    return m_d->maxFreq;
}

void FreqEditWidget::setMaxFreq(long int freq)
{
    if (m_d->maxFreq == freq) {
        return;
    }

    m_d->maxFreq = freq;
    m_d->updateDigitWidgets();
    m_d->update();
}

long int FreqEditWidget::minFreq() const
{
    return m_d->minFreq;
}

void FreqEditWidget::setMinFreq(long int freq)
{
    m_d->minFreq = freq;
    m_d->update();
}

long int FreqEditWidget::freq() const
{
    return m_d->freq;
}

FreqEditWidget::Freqs FreqEditWidget::getPresetFreqs() const noexcept
{
    return m_d->presetFreqs;
}

std::vector<long> FreqEditWidget::getPresetFreqsVector() const noexcept
{
    std::vector<long> dst;
    Freqs::const_iterator it{m_d->presetFreqs.begin()};
    Freqs::const_iterator end{m_d->presetFreqs.end()};

    for (; it != end; ++it) {
        dst.push_back(it.value().second.first);
    }

    return dst;
}

bool FreqEditWidget::canSetFreq() const noexcept
{
    return !m_d->m_lastModified.isActive();
}

/*! Устанавливает мпч и орч*/
void FreqEditWidget::setForecastingRange(int mpch, int orch)
{
    m_d->setForecastingRange(mpch, orch);
}

void FreqEditWidget::setEditing(bool edit)
{
    m_d->freqEditWidget->setEnabled(edit);
}

void FreqEditWidget::setFreq(long int freq, bool force)
{
    if (force || canSetFreq()) {
        m_d->setFreq(freq, true);
    }
}

void FreqEditWidget::setPresetFreqs(const Freqs& freqs)
{
    m_d->presetFreqs = freqs;
    Freqs::iterator it;
    for (it = m_d->presetFreqs.begin(); it != m_d->presetFreqs.end(); ++it) {
        if (m_d->freq == it.value().second.first) {
            m_d->currPresetFreqNum = it.key();
        }
    }
    m_d->updatePresetFreqsWidget();
}

void FreqEditWidget::setFreqColor(QColor color)
{
    m_d->setFreqColor(color);
}

void FreqEditWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    m_d->updateSize(event->size());
}

void FreqEditWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    m_d->updateSize(size());
}

void FreqEditWidget::paintEvent(QPaintEvent* /*event*/)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void FreqEditWidget::onChangedDigit(QWidget* sender, int symbolNumber, bool incFlag)
{
    if (auto* digitWidget = dynamic_cast<DigitWidget*>(sender)) {
        int digitOrder = digitWidget->symbolCount() - symbolNumber - 1;
        if (digitWidget == m_d->majorDigitWidget) {
            digitOrder += m_d->minorDigitWidget->symbolCount();
        }

        if (digitOrder >= 0) {
            int n = 1;
            for (; digitOrder > 0; --digitOrder) {
                n *= 10;
            }

            if (incFlag) {
                m_d->setFreq(m_d->freq + n, false);
            } else {
                m_d->setFreq(m_d->freq - n, false);
            }
        }
    }
}

void FreqEditWidget::onNextPresetFreq()
{
    if ((m_d->currPresetFreqNum + 1) < m_d->presetFreqs.size()) {
        int freq = m_d->presetFreqs.value(++m_d->currPresetFreqNum).second.first;
        m_d->setFreq(freq, true);
        emit presetFreqChanged(m_d->currPresetFreqNum);
    }
}

void FreqEditWidget::onPredPresetFreq()
{
    if (m_d->currPresetFreqNum > 0) {
        int freq = m_d->presetFreqs.value(--m_d->currPresetFreqNum).second.first;
        m_d->setFreq(freq, true);
        emit presetFreqChanged(m_d->currPresetFreqNum);
    }
}

void FreqEditWidget::onSelectPresetFreq()
{
    if (auto* action = dynamic_cast<QAction*>(sender())) {
        const QVariant& freqNumVar = action->data();
        int freqNum = freqNumVar.toInt();
        if (0 <= freqNum && freqNum < m_d->presetFreqs.size()) {
            m_d->setFreq(m_d->presetFreqs.value(freqNum).second.first, true);
            emit presetFreqChanged(freqNum);
        }
    }
}

void FreqEditWidget::onDigitChanged(QString /*unused*/)
{
    m_d->calcAndSetFreq();
}
