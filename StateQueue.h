#pragma once

#include <exception>
#include <type_traits>

template <class Q> class StateQueue : public Q
{
public:
    using base_t = Q;
    using value_type = typename Q::value_type;

    enum class State { START, STOPPING, STOPPED };

    class Failure : public std::exception {};
    class QStopping : public Failure {};
    class QStopped : public Failure {};
    class Empty : public Failure {};

    ~StateQueue()
    {
        Stop();
    }

    State state() const { return state_; }

    void Start()
    {
        if (state_ == State::START) { return; }

        state_ = State::START;
    }

    void Stopping()
    {
        if (state_ == State::STOPPING) { return; }
        if (state_ == State::STOPPED) { return; }

        state_ = State::STOPPING;
        if (!base_t::empty()) { return; }

        state_ = State::STOPPED;
    }

    void Stop()
    {
        if (state_ == State::STOPPED) { return; }

        Clear();
        state_ = State::STOPPED;
    }

    void Clear()
    {
        auto tmp = base_t();
        base_t::swap(tmp);
    }

    void push(
        _In_ const value_type& v)
    {
        if (state_ == State::STOPPED) {
            throw QStopped();
        }
        if (state_ == State::STOPPING) {
            throw QStopping();
        }

        base_t::push(v);
    }

    void push(
        _Inout_ value_type&& v)
    {
        if (state_ == State::STOPPED) {
            throw QStopped();
        }
        if (state_ == State::STOPPING) {
            throw QStopping();
        }

        base_t::push(std::move(v));
    }

    auto front()
    {
        if (state_ == State::STOPPED) {
            throw QStopped();
        }

        if (base_t::empty()) { throw Empty(); }

        return front<has_front<Q>::value>();
    }

    void pop()
    {
        if (state_ == State::STOPPED) {
            throw QStopped();
        }

        base_t::pop();

        if (state_ == State::STOPPING) {
            if (base_t::empty()) {
                state_ = State::STOPPED;
            }
        }
    }

protected:
    State state_ = State::START;

private:
    template <typename, typename = std::void_t<>>
    struct has_front : std::false_type {};

    template <typename Y>
    struct has_front <Y, std::void_t<decltype(std::declval<Y>().front())>> : std::true_type {};

    // bool for front exists or not
    template <bool>
    auto front();

    template <>
    auto front<true>()
    {
        // for queue
        return base_t::front();
    }

    template <>
    auto front<false>()
    {
        // for priority_queue
        return base_t::top();
    }
};
