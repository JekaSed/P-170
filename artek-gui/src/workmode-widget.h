#ifndef WORKMODEWIDGET_H
#define WORKMODEWIDGET_H

#include "src/cloud/cloud.h"

#include <QComboBox>

class WorkModeWidget : public Cloud
{
    Q_OBJECT
public:
    WorkModeWidget(QWidget* p);
    QJsonObject toJsonObj() const;
    void fromJsonObj(const QJsonObject& obj);

private:
    class WorkModeData* m_data;
    QComboBox* m_emissionCb;
    QComboBox* m_deviationCb;
    QComboBox* m_bitrateCb;
    QComboBox* m_sideBandCb;

private:
    void updateDeviations();
    void updateBitrates();
    void emitChangedSignal();

signals:
    void changed(const QJsonObject&);

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif   // WORKMODEWIDGET_H
