#pragma once

#include <exception>
#include <type_traits>

template <class V> class StateVector : public V
{
public:
    using base_t = V;
    using value_type = typename V::value_type;

    enum State : unsigned int
    {
        NONE =0x0, START = 0x1, STOPPING = 0x2, STOPPED = 0x4
    };

    class Failure : public std::exception {};
    class VStopping : public Failure {};
    class VStopped : public Failure {};
    class Empty : public Failure {};

    ~StateVector()
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
        base_t::clear();
    }

protected:
    State state_ = State::START;
};

template <class V>
using StateVectorState = typename StateVector<V>::State;

template <class V>
StateVectorState<V> operator &(StateVectorState<V> lhs, StateVectorState<V> rhs) noexcept
{
    using U = std::underlying_type_t<StateVectorState<V>>;
    return static_cast<StateVectorState<V>>(static_cast<U>(lhs) & static_cast<U>(rhs));
}
