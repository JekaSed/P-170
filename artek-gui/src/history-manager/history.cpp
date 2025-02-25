#include "history.h"
#include <qjsondocument.h>

void History::setInitialDoc(const QJsonDocument& doc)
{
    m_currentState = doc;
    clear();
}

void History::push(const QJsonDocument& d)
{
    m_undoActions.push(m_currentState);
    m_currentState = d;
    m_redoActions.clear();
}

void History::clear()
{
    m_undoActions.clear();
    m_redoActions.clear();
}

QJsonDocument History::undo()
{
    if (m_undoActions.isEmpty()) {
        return {};
    }
    auto res = m_undoActions.pop();
    m_redoActions.push(m_currentState);
    m_currentState = res;
    return res;
}

QJsonDocument History::redo()
{
    if (m_redoActions.isEmpty()) {
        return {};
    }
    auto res = m_redoActions.pop();
    m_undoActions.push(m_currentState);
    m_currentState = res;
    return res;
}

std::size_t History::undoSize() const noexcept
{
    return m_undoActions.size();
}

std::size_t History::redoSize() const noexcept
{
    return m_redoActions.size();
}
