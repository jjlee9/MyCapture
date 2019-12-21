#pragma once

#include <mutex>
#include <condition_variable>
#include "Monitor.h"
#include "StateQueue.h"

// SQ should be a StateQueue, M should be a mutex type, C should be a condition variable
// You can not uniqueInvoke an uniqueInvoke method if your M = std::mutex & C = std::condition_variable.
// You can try std::shared_timed_mutex and std::condition_variable_any.
template <class SQ, class M = std::mutex, class C = std::condition_variable> class MonitorQueue : public Monitor<SQ, M>
{
public:
    using base_t = Monitor<SQ, M>;
    using value_type = typename SQ::value_type;
    using mutex_t = typename base_t::mutex_t;
    using Failure = typename SQ::Failure;
    using QStopping = typename SQ::QStopping;
    using QStopped = typename SQ::QStopped;
    using Empty = typename SQ::Empty;

    ~MonitorQueue()
    {
        Stop();
    }

    void Start()
    {
        if (base_t::state_ == SQ::State::START) { return; }

        base_t::uniqueInvoke([this]
            {
                base_t::Start();
            });
        queueChanged_.notify_all();
    }

    void Stopping()
    {
        if (base_t::state_ == SQ::State::STOPPING) { return; }
        if (base_t::state_ == SQ::State::STOPPED) { return; }

        base_t::uniqueInvoke([this]
            {
                base_t::Stopping();
            });
        if (base_t::state_ == SQ::State::STOPPED) {
            queueChanged_.notify_all();
        }
    }

    void Stop()
    {
        if (base_t::state_ == SQ::State::STOPPED) { return; }

        base_t::uniqueInvoke([this]
            {
                base_t::Stop();
            });
        if (base_t::state_ == SQ::State::STOPPED) {
            queueChanged_.notify_all();
        }
    }

    void Clear()
    {
        base_t::uniqueInvoke([this]
            {
                base_t::Clear();
            });
        queueChanged_.notify_all();
    }
    void push(
        _In_ const value_type& v)
    {
        base_t::uniqueInvoke([this, &v]
            {
                base_t::push(v);
            });
        queueChanged_.notify_one();
    }

    void push(
        _Inout_ value_type&& v)
    {
        base_t::uniqueInvoke([this, &v]
            {
                base_t::push(std::move(v));
            });
        queueChanged_.notify_one();
    }

    value_type front()
    {
        std::unique_lock<mutex_t> lock(base_t::mutex_);
        for (; (base_t::state_ != SQ::State::STOPPED) && base_t::empty();) {
            queueChanged_.wait(lock);
        }

        return base_t::front();
    }

    void pop()
    {
        base_t::uniqueInvoke([this]
            {
                base_t::pop();
            });
        queueChanged_.notify_one();
    }

    // an exception unsafe way
    auto Pop()
    {
        decltype(base_t::front()) v;

        {
            std::unique_lock<mutex_t> lock(base_t::mutex_);
            for (; (base_t::state_ != SQ::State::STOPPED) && base_t::empty();) {
                queueChanged_.wait(lock);
            }

            v = base_t::front();
            base_t::pop();
        }

        queueChanged_.notify_one();
        return v;
    }

    void WaitChanged()
    {
        std::unique_lock<mutex_t> lock(base_t::mutex_);
        for (; (base_t::state_ != SQ::State::STOPPED) && base_t::empty();) {
            queueChanged_.wait(lock);
        }
    }

protected:
    using cond_t = C;

    cond_t queueChanged_;
};
