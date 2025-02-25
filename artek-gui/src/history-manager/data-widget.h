#pragma once

#include <qjsondocument.h>
#include <qwidget.h>

class RadioDataWidget : public QWidget
{
    Q_OBJECT

public:
    RadioDataWidget(QWidget* parent = nullptr)
      : QWidget(parent)
    {}

    /**
     * @brief Идентификатор радиоданных
     *
     * @return QString
     */
    virtual QString dataName() const noexcept = 0;
    virtual QJsonDocument currentDocument() const noexcept = 0;

public slots:
    /**
     * @brief загрузка виджета по JSON документу
     *
     */
    virtual void updateFromDocument(QJsonDocument) = 0;

    virtual void saveCurrentDocument() = 0;

signals:
    void documentChanged(QJsonDocument);
};
