#pragma once

#include <QElapsedTimer>
#include <chrono>
#include <qobject.h>

constexpr int defaultStateTimeout = 500;

template<typename T>
class Delayed
{
public:
    virtual ~Delayed() = default;

    explicit Delayed()
      : m_timeout(defaultStateTimeout)
    {
        m_elapsed.start();
    }

    void setState(const T& newState)
    {
        if (m_elapsed.elapsed() > m_timeout) {
            setNewState(newState);
        }
    }

    /**
     * @brief заморозить обновление
     *
     * @param stateTimeout время заморозки
     */
    void freeze(std::chrono::milliseconds stateTimeout = std::chrono::milliseconds(defaultStateTimeout))
    {
        m_timeout = static_cast<int>(stateTimeout.count());
        m_elapsed.start();
    }

    void unfreeze()
    {
        freeze(std::chrono::milliseconds(0));
    }

protected:
    virtual void setNewState(const T& newState) = 0;

private:
    QElapsedTimer m_elapsed;
    int m_timeout;
};
