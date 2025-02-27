#pragma once

#include <exception>
#include <functional>

#include <QDebug>

struct Defer
{
    Defer(std::function<void()> f)
      : m_fn(std::move(f))
    {}

    ~Defer()
    {
        try {
            m_fn();
        } catch (const std::exception& e) {
            qWarning() << "defer:" << e.what();
        } catch (...) {
            qWarning() << "defer: unprocessed error";
        }
    }

private:
    std::function<void()> m_fn;
};
