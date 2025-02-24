#pragma once

#include <QtWidgets/QComboBox>
#include <QColor>
#include <qglobal.h>

class QtMaterialComboBoxPrivate;

class QtMaterialComboBox : public QComboBox
{
    Q_OBJECT

    Q_PROPERTY(QColor textColor WRITE setTextColor READ textColor)
    Q_PROPERTY(QColor inkColor WRITE setInkColor READ inkColor)

public:
    explicit QtMaterialComboBox(QWidget* parent = 0);
    ~QtMaterialComboBox();

    void setUseThemeColors(bool value);
    bool useThemeColors() const;

    void setLabelFontSize(qreal size);
    qreal labelFontSize() const;

    void setLabel(const QString& label);
    QString labelText() const;

    void setTextColor(const QColor& color);
    QColor textColor() const;

    void setLabelColor(const QColor& color);
    QColor labelColor() const;

    void setInkColor(const QColor& color);
    QColor inkColor() const;

    void setInputLineColor(const QColor& color);
    QColor inputLineColor() const;

    void setIgnoreWheel(const bool ignore);

    void showPopup() Q_DECL_OVERRIDE;

private:
    void updateItems();
    bool dataIsChanged() const;

protected:
    QtMaterialComboBox(QtMaterialComboBoxPrivate& d, QWidget* parent = 0);

    bool event(QEvent* event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

    const QScopedPointer<QtMaterialComboBoxPrivate> d_ptr;

private:
    Q_DISABLE_COPY(QtMaterialComboBox)
    Q_DECLARE_PRIVATE(QtMaterialComboBox)
};
