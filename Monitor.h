#pragma once

#include <shared_mutex>

// I cannot use std::shared_mutex here! boost/thread/pthread/shared_mutex.hpp?
template <class T, class M = std::shared_timed_mutex> class Monitor : public T
{
public:
    using base_t = T;
    using mutex_t = M;

    template <typename F, typename... Args>
    auto uniqueInvoke(
        F&&       f,
        Args&&... args) const
    {
        std::lock_guard<mutex_t> lock(mutex_); // or std::unique_lock
        return f(args...);
    }

    template <typename F, typename... Args>
    auto sharedInvoke(
        F&&       f,
        Args&&... args) const
    {
        std::shared_lock<mutex_t> lock(mutex_); // or std::unique_lock
        return f(args...);
    }

protected:
    mutable mutex_t mutex_;
};
