#include "text-dialog.h"
#include "src/button/flat-button.h"
#include "src/theme/theme.h"
#include <qabstractbutton.h>
#include <qboxlayout.h>
#include <qdialog.h>
#include <qeventloop.h>
#include <qfontmetrics.h>
#include <qinputdialog.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qparallelanimationgroup.h>
#include <qpen.h>
#include <qpoint.h>
#include <qpropertyanimation.h>
#include <qstyle.h>
#include <qpainter.h>
#include <qtmaterialtextfield.h>

TextDialog::TextDialog(const QString& title, const QString& label, QWidget* parent)
  : BaseDialog(title, parent)
{
    setTitleVisible(false);
    auto* okBt = makeFlatButton(tr("Применить"), style()->standardIcon(QStyle::SP_DialogApplyButton), this);
    auto* cancelBt = makeFlatButton(tr("Отменить"), style()->standardIcon(QStyle::SP_DialogCancelButton), this);

    auto te = new QtMaterialTextField(this);
    te->setLabel(title);
    te->setPlaceholderText(label);
    m_te = te;

    connect(cancelBt, &QAbstractButton::clicked, this, &QInputDialog::reject);
    connect(okBt, &QAbstractButton::clicked, this, &QInputDialog::accept);

    auto* btLay = new QHBoxLayout;
    btLay->addWidget(cancelBt);
    btLay->addWidget(okBt);

    auto* l = new QVBoxLayout(this);
    l->addWidget(te);
    l->addLayout(btLay);
}

QString TextDialog::text() const
{
    return m_te->text();
}

QString TextDialog::getText(const QString& title, const QString& label, QWidget* parent)
{
    TextDialog dial(title, label, parent);
    return dial.exec() == QInputDialog::Accepted ? dial.text() : QString{};
}

BaseDialog::BaseDialog(const QString& title, QWidget* parent)
  : QDialog(parent)
  , m_animation(new QParallelAnimationGroup(this))
  , m_colorAnimation(new QPropertyAnimation(this, "titleColor", this))
  , m_lineColor(theme::errorColor())
{
    constexpr int minWidth{400};
    setMinimumWidth(minWidth);
    setWindowFlags(Qt::WindowType::FramelessWindowHint | Qt::WindowType::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);

    constexpr int animDuration{5000};
    auto* titleAnim = new QPropertyAnimation(this, "progress", this);
    titleAnim->setStartValue(0.0);
    titleAnim->setKeyValueAt(0.5, 1);
    titleAnim->setEndValue(0.0);
    titleAnim->setDuration(animDuration);

    m_colorAnimation->setStartValue(theme::textColor());
    m_colorAnimation->setKeyValueAt(0.5, theme::errorColor());
    m_colorAnimation->setEndValue(theme::textColor());
    m_colorAnimation->setDuration(animDuration);

    auto* g = static_cast<QParallelAnimationGroup*>(m_animation);
    g->addAnimation(titleAnim);
    g->addAnimation(m_colorAnimation);
    g->setLoopCount(-1);
    g->start();

    setContentsMargins(0, fontMetrics().height() + 5, 0, 0);
    setWindowTitle(title);
}

void BaseDialog::setTitleVisible(bool visible)
{
    m_titleIsVisible = visible;
    if (visible) {
        m_animation->start();
    } else {
        m_animation->stop();
    }
    const auto topPad = visible ? fontMetrics().height() + 5 : 0;
    setContentsMargins(0, topPad, 0, 0);
    update();
}

QColor BaseDialog::titleColor() const
{
    return m_titleColor;
}

void BaseDialog::setTitleColor(QColor col)
{
    m_titleColor = col;
    update();
}

qreal BaseDialog::progress() const
{
    return m_progress;
}

void BaseDialog::setProgress(qreal progress)
{
    m_progress = progress;
    update();
}

void BaseDialog::paintEvent(QPaintEvent* /*p*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(theme::topMenuColor());
    constexpr int radius{15};
    p.drawRoundedRect(rect(), radius, radius);

    if (!m_titleIsVisible) {
        return;
    }
    // draw title
    p.setPen(QPen(m_titleColor, 1));
    constexpr int padding{4};
    auto f = p.font();
    f.setBold(true);
    p.setFont(f);
    QRectF titleRect(padding, padding, width() - padding, p.fontMetrics().height());
    p.drawText(titleRect, Qt::AlignHCenter, windowTitle());
    p.setPen(QPen(m_lineColor, 2));

    const auto w = (1.0 - m_progress) * titleRect.width() * 0.3;
    p.setOpacity(m_progress);
    titleRect.adjust(w, 0, -w, 0);
    p.drawLine(titleRect.bottomLeft() + QPoint(radius, 4), titleRect.bottomRight() + QPoint(-radius, 4));
}

ErrorDialog::ErrorDialog(const QString& title, QWidget* parent)
  : BaseDialog(title, parent)
{
    auto* okBt = makeFlatButton(tr("Принято"), style()->standardIcon(QStyle::SP_MessageBoxWarning), this);
    connect(okBt, &QAbstractButton::clicked, this, &ErrorDialog::accept);

    auto* l = new QVBoxLayout(this);
    l->addWidget(new QLabel(""));
    l->addWidget(okBt);
}

void ErrorDialog::setText(const QString& errorText)
{
    findChild<QLabel*>()->setText(errorText);
}

void ErrorDialog::showError(const QString& title, const QString& errorText, QWidget* parent)
{
    ErrorDialog dial(title, parent);
    dial.setText(errorText);
    dial.exec();
}

QuestionDialog::QuestionDialog(const QString& title, QWidget* parent, const QPair<QString, QString>& yesNo)
  : BaseDialog(title, parent)
{
    QPair<QString, QString> btTexts;
    if (!yesNo.first.isEmpty() && !yesNo.second.isEmpty()) {
        btTexts.first = yesNo.first;
        btTexts.second = yesNo.second;
    } else {
        btTexts.first = tr("Да");
        btTexts.second = tr("Нет");
    }
    m_yesBt = makeFlatButton(btTexts.first, style()->standardIcon(QStyle::SP_DialogYesButton), this);
    m_noBt = makeFlatButton(btTexts.second, style()->standardIcon(QStyle::SP_DialogNoButton), this);
    connect(m_noBt, &QAbstractButton::clicked, this, &QInputDialog::reject);
    connect(m_yesBt, &QAbstractButton::clicked, this, &QInputDialog::accept);

    m_lineColor = theme::iconColor();
    m_colorAnimation->setStartValue(theme::iconColor());
    m_colorAnimation->setKeyValueAt(0.5, theme::textColor());
    m_colorAnimation->setEndValue(theme::iconColor());

    auto* btLay = new QHBoxLayout;
    btLay->addWidget(m_noBt);
    btLay->addWidget(m_yesBt);

    auto* l = new QVBoxLayout(this);
    l->addWidget(new QLabel(this));
    l->addLayout(btLay);
}

void QuestionDialog::setText(const QString& questionText)
{
    findChild<QLabel*>()->setText(questionText);
}

bool QuestionDialog::showQuestion(const QString& title, const QString& questionText, QWidget* parent,
                                  const QPair<QString, QString>& yesNo)
{
    QuestionDialog dial(title, parent, yesNo);
    dial.setText(questionText);
    return dial.exec() == QInputDialog::Accepted;
}

InformationDialog::InformationDialog(const QString& title, QWidget* parent)
  : BaseDialog(title, parent)
{
    auto* okBt = makeFlatButton(tr("Принято"), style()->standardIcon(QStyle::SP_MessageBoxInformation), this);
    connect(okBt, &QAbstractButton::clicked, this, &ErrorDialog::accept);

    m_lineColor = theme::iconColor();
    m_colorAnimation->setStartValue(theme::iconColor());
    m_colorAnimation->setKeyValueAt(0.5, theme::textColor());
    m_colorAnimation->setEndValue(theme::iconColor());

    auto* l = new QVBoxLayout(this);
    l->addWidget(new QLabel(""));
    l->addWidget(okBt);
}

void InformationDialog::setText(const QString& info)
{
    findChild<QLabel*>()->setText(info);
}

void InformationDialog::showInfo(const QString& title, const QString& info, QWidget* parent)
{
    InformationDialog dial(title, parent);
    dial.setText(info);
    dial.exec();
}
