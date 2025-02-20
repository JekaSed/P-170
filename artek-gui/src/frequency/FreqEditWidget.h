#pragma once

#include <QtCore/QList>
#include <QWidget>
#include <qcolor.h>
#include <qevent.h>

class FreqEditWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(long int maxFreq READ maxFreq WRITE setMaxFreq)
    Q_PROPERTY(long int minFreq READ minFreq WRITE setMinFreq)
    Q_PROPERTY(long int freq READ freq WRITE setFreq)

public:
    using Freqs = QMap<long int, QPair<bool, QPair<int, std::string>>>;
    static const int DefaultMinFreq;
    static const int DefaultMaxFreq;
    static const int DefaultFreq;

public:
    explicit FreqEditWidget(QWidget* parent = nullptr);
    ~FreqEditWidget() override;

    long int maxFreq() const;
    void setMaxFreq(long int freq);

    long int minFreq() const;
    void setMinFreq(long int freq);

    long int freq() const;
    Freqs getPresetFreqs() const noexcept;
    std::vector<long> getPresetFreqsVector() const noexcept;

    void setForecastingRange(int mpch, int orch);

    void setEditing(bool edit);

public slots:
    void setFreq(long int freq, bool force = false);

    void setPresetFreqs(const Freqs& freqs);

    void setFreqColor(QColor color);

public:   // QWidget
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

signals:
    void freqEdited(long int freq);
    void freqChanged(long int freq);
    void presetFreqChanged(long int pos);

private slots:
    void onChangedDigit(QWidget* sender, int symbolNumber, bool incFlag);

    void onNextPresetFreq();
    void onPredPresetFreq();
    void onSelectPresetFreq();

    void onDigitChanged(QString);

private:
    bool canSetFreq() const noexcept;
    class FreqEditWidgetPrivate;
    friend class FreqEditWidgetPrivate;
    FreqEditWidgetPrivate* m_d;
};
