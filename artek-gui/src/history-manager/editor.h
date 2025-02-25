#pragma once

#include "src/cloud/cloud.h"
#include "data-widget.h"
#include "history.h"
#include <qjsondocument.h>
#include <qpushbutton.h>

class RadioDataEditor : public Cloud
{
    Q_OBJECT

public:
    RadioDataEditor(const QString& title, QWidget* p);

public slots:
    void setRadioDataWidget(RadioDataWidget* rd);

    void hideControls();
    void showControls();
    void resetLocalStorage();

    QJsonDocument loadLastDocument() const;
    bool hasUnsavedChanges() const noexcept;

private slots:
    void updateHistory(const QJsonDocument& doc);
    void checkControls();
    void makeCtrZ();
    void makeCtrY();
    void makeCtrS();

private:
    void saveTempActiveDoc(const QJsonDocument& doc);

private:
    QWidget* m_controlWidget;
    QPushButton* m_undoBt;
    QPushButton* m_redoBt;
    QPushButton* m_saveBt;
    QString m_radioTitle;
    History m_history;
    RadioDataWidget* m_radioData{nullptr};
};
