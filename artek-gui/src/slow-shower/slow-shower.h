#pragma once

#include <QWidget>

class SlowShower : public QWidget
{
    Q_OBJECT

public:
    SlowShower(QWidget* parent = nullptr);
public slots:
    void setVisible(bool visible) override;

private:
    class QPropertyAnimation* m_showAnim;
    bool m_needShow{true};
};
