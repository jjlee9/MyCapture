#pragma once

#include <exception>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <limits>
#include "Monitor.h"

// SV should be a StateVector, M should be a mutex type, C should be a condition variable
// You can not uniqueInvoke an uniqueInvoke method if your M = std::mutex & C = std::condition_variable.
// You can try std::shared_timed_mutex and std::condition_variable_any.
template <class SV, class M = std::mutex, class C = std::condition_variable> class MonitorModuloVector : public Monitor<SV, M>
{
public:
    using base_t = Monitor<SV, M>;
    using value_type = typename SV::value_type;
    using reference = typename SV::reference;
    using const_reference = typename SV::const_reference;
    using mutex_t = typename base_t::mutex_t;
    using size_type = unsigned int;
    using State = typename SV::State;

    class Failure : public std::exception {};
    class OutOfRange : public Failure {};

    ~MonitorModuloVector()
    {
        Stop();
    }

    bool Init(
        _In_ size_type modulus,
        _In_ size_type upperBoundIndex)
    {
        base_t::resize(modulus);
        upperBoundIndex_ = upperBoundIndex;
        SetStartIndex(0);

        return true;
    }

    void Start()
    {
        if (base_t::state_ == State::START) { return; }

        base_t::uniqueInvoke([this]
            {
                base_t::Start();
            });
    }

    void Stopping()
    {
        if (base_t::state_ == State::STOPPING) { return; }
        if (base_t::state_ == State::STOPPED) { return; }

        base_t::uniqueInvoke([this]
            {
                base_t::Stopping();
            });
        if (base_t::state_ == State::STOPPED) {
            vectorReader_.notify_all();
            vectorWriter_.notify_all();
        }
    }

    void Stop()
    {
        if (base_t::state_ == State::STOPPED) { return; }

        base_t::uniqueInvoke([this]
            {
                base_t::Stop();
            });
        vectorReader_.notify_all();
        vectorWriter_.notify_all();
    }

    auto CurrentElements()
    {
        return currElements_.load();
    }

    bool InRange(
        _In_ size_type index) const
    {
        auto startIndex = startIndex_.load();
        auto endIndex = endIndex_.load();

        if (index < startIndex) { return false; }
        if (index >= endIndex) { return false; }

        return true;
    }

    // wait until the index is fall in the range of [startIndex_, endIndex_ ) or 
    void WaitInRange(
        _In_ size_type index,
        _In_ State     waitState)
    {
        std::unique_lock<mutex_t> lock(base_t::mutex_);
        for (; ((base_t::state_ & waitState) != State::NONE) && !InRange(index);) {
            vectorReader_.wait(lock);
        }
    }

    void SetStartIndex(
        _In_ size_type startIndex)
    {
        base_t::uniqueInvoke([startIndex, this]
            {
                startIndex_ = startIndex;
                auto endIndex = startIndex + static_cast<size_type>(base_t::size());
                if (endIndex > upperBoundIndex_) {
                    endIndex = upperBoundIndex_;
                }
                endIndex_ = endIndex;
            });
    }

    reference operator [](
        _In_ size_type index)
    {
        if (!InRange(index)) { throw OutOfRange(); }

        auto moduloIndex = index % base_t::size();
        return base_t::operator [](moduloIndex);
    }

    const_reference operator [](
        _In_ size_type index) const
    {
        if (!InRange(index)) { throw OutOfRange(); }

        auto moduloIndex = index % base_t::size();
        return base_t::operator [](moduloIndex);
    }

    bool IsEmpty() const
    {
        return currElements_ == 0;
    }

    void WaitEmpty(
        _In_ State waitState)
    {
        std::unique_lock<mutex_t> lock(base_t::mutex_);
        for (; ((base_t::state_ & waitState) != State::NONE) && !IsEmpty();) {
            vectorReader_.wait(lock);
        }
    }

    auto DecrementOneElement()
    {
        return --currElements_;
    }

    template <class F>
    void CheckNotifyEmpty(
        _In_ F&& f)
    {
        if (IsEmpty()) {
            f();
            vectorReader_.notify_all();
        }
    }

    bool IsFull() const
    {
        size_type startIndex, endIndex, currElements;

        base_t::uniqueInvoke([&startIndex, &endIndex, &currElements, this]
            {
                startIndex = startIndex_;
                endIndex = endIndex_;
                currElements = currElements_;
            });

        return currElements == (endIndex - startIndex);
    }

    void WaitFull(
        _In_ State waitState)
    {
        std::unique_lock<mutex_t> lock(base_t::mutex_);
        for (; ((base_t::state_ & waitState) != State::NONE) && !IsFull();) {
            vectorWriter_.wait(lock);
        }
    }

    auto IncrementOneElement()
    {
        return ++currElements_;
    }

    template <class F>
    void CheckNotifyFull(F&& f)
    {
        if (IsFull()) {
            vectorWriter_.notify_all();
            f();
        }
    }

    void NextModulusBlock()
    {
        SetStartIndex(startIndex_.load() + static_cast<size_type>(base_t::size()));
    }

protected:
    using cond_t = C;

    std::atomic<size_type> startIndex_ = 0;
    std::atomic<size_type> endIndex_ = 0;     // excluded
    std::atomic<size_type> currElements_ = 0; // <= (endIndex_ - startIndex)
    size_type              upperBoundIndex_ = (std::numeric_limits<size_type>::max)();
    cond_t                 vectorReader_;
    cond_t                 vectorWriter_;
};
