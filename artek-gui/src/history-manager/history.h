#pragma once

#include <cstddef>
#include <qjsondocument.h>
#include <qstack.h>

class History
{
public:
    void setInitialDoc(const QJsonDocument& doc);
    void push(const QJsonDocument& d);
    void clear();

    QJsonDocument undo();

    inline QJsonDocument ctrlZ()
    {
        return undo();
    }

    QJsonDocument redo();

    inline QJsonDocument ctrlY()
    {
        return redo();
    }

    std::size_t undoSize() const noexcept;
    std::size_t redoSize() const noexcept;

private:
    QJsonDocument m_currentState;
    // предыдущие состояния Ctrl+Z
    QStack<QJsonDocument> m_undoActions;

    // отмененные действия  состояния Ctrl+Y
    QStack<QJsonDocument> m_redoActions;
};
