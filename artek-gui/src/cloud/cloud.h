#pragma once

#include "src/theme/theme.h"
#include <qabstractbutton.h>
#include <qboxlayout.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qwidget.h>

class Cloud : public QWidget
{
    Q_OBJECT
public:
    Cloud(const QString& title, QWidget* p, const QColor& bgColor = theme::foregroundColor(),
          QPair<qreal, qreal> collapseDegrees = {0.0, 180.0});
    ~Cloud() override;
    QHBoxLayout* titleLayout();
    QVBoxLayout* contentLayout();

    bool contentIsVisible() const noexcept;

    QWidget* contentWidget() const;

    void bindCloud(Cloud* other);
    QString titleText() const;

public slots:
    void setStyle(QColor border, Qt::PenStyle style = Qt::PenStyle::SolidLine,
                  QColor bgColor = theme::foregroundColor());
    void setExpandVisible(bool visible);
    void setContentVisible(bool visible);
    void setTitle(const QString& title);

    void setTitleVisible(bool visible);

signals:
    void contentVisibilityChanged(bool);

protected:
    QLabel* m_titleLb;

private:
    QVBoxLayout* m_vTitle{nullptr};
    QWidget* m_titleWidget;
    QHBoxLayout* m_hTitle;
    QWidget* m_contentWidget;
    QVBoxLayout* m_content;
    QAbstractButton* m_expand;
};
