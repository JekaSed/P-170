#pragma once

#include <QtCore/QChar>
#include <QWidget>
#include <QChar>

class QKeyEvent;

class LCDWidget : public QWidget
{
    Q_OBJECT

public:
    static const QChar InvalidChar;

public:
    explicit LCDWidget(QWidget* parent = nullptr);
    ~LCDWidget() override;

    void setSymbolCount(int count);
    int symbolCount() const;

    enum Type
    {
        POINT = 0,
        COLON
    };

    void setSymbol(int pos, QChar sym, bool on = true, Type type = POINT);
    QChar symbol(int pos, bool* on = nullptr) const;

    void enablePoint(int pos, bool enable = true, bool on = true, Type type = POINT);
    bool isEnabledPoint(int pos, Type& type, bool* on = nullptr) const;

    QRect symbolRect(int pos) const;

    void setDrawOffSegments(bool enable);
    bool hasDrawOffSegments() const;

    void setHeightHint(int h);
    int widthForHeight(int h) const;

    void setOnColor(const QBrush& color);

    void setEnabled(bool enabled);

    QString toString() const noexcept;

    /*!
     * @brief привязывает другой виджет, образуя единое целое
     *
     * @param lcd виджет
     */
    void attachRightLcd(LCDWidget* lcd);

    void setInteractive(bool enable);

    void setRelationship(bool enable);

    void setShowLeadingZeroes(bool show);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void valueChanged(QString);

protected:   // QWidget
    void paintEvent(QPaintEvent* event) override;
    bool event(QEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    bool processKeyEvent(QKeyEvent* event);
    class LCDWidgetPrivate;
    LCDWidgetPrivate* m_d;
};
