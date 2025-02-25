#pragma once

#include <QDialog>
#include <qabstractbutton.h>
#include <qcolor.h>
#include <qdialog.h>
#include <qwidget.h>

class BaseDialog : public QDialog
{
    Q_OBJECT

    Q_PROPERTY(qreal progress READ progress WRITE setProgress)
    Q_PROPERTY(QColor titleColor WRITE setTitleColor READ titleColor)
public:
    explicit BaseDialog(const QString& title, QWidget* parent);
    void setTitleVisible(bool visible);
    QColor titleColor() const;
    void setTitleColor(QColor col);

private:
    qreal progress() const;
    void setProgress(qreal progress);

protected:
    void paintEvent(QPaintEvent* /*p*/) override;

private:
    bool m_titleIsVisible{true};
    qreal m_progress{};
    QColor m_titleColor{Qt::white};

    class QAbstractAnimation* m_animation;

protected:
    class QPropertyAnimation* m_colorAnimation;
    QColor m_lineColor;
};

class TextDialog : public BaseDialog
{
    Q_OBJECT
public:
    TextDialog(const QString& title, const QString& label, QWidget* parent);
    QString text() const;

    static QString getText(const QString& title, const QString& label, QWidget* parent);

private:
    class QLineEdit* m_te;
};

class ErrorDialog : public BaseDialog
{
    explicit ErrorDialog(const QString& title, QWidget* parent = nullptr);

    void setText(const QString& errorText);

public:
    static void showError(const QString& title, const QString& errorText, QWidget* parent);
};

class QuestionDialog : public BaseDialog
{
    explicit QuestionDialog(const QString& title, QWidget* parent = nullptr, const QPair<QString, QString>& yesNo = {});

    void setText(const QString& questionText);

public:
    static bool showQuestion(const QString& title, const QString& questionText, QWidget* parent,
                             const QPair<QString, QString>& yesNo = {});

private:
    QAbstractButton* m_yesBt;
    QAbstractButton* m_noBt;
};

class InformationDialog : public BaseDialog
{
    explicit InformationDialog(const QString& title, QWidget* parent = nullptr);

    void setText(const QString& info);

public:
    static void showInfo(const QString& title, const QString& info, QWidget* parent);
};
