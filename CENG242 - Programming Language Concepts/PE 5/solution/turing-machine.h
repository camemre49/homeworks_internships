#pragma once
#include "turing-machine_utils.h"

template <typename A, typename B> struct is_same { static constexpr auto value = false; };
template <typename A> struct is_same<A, A>{ static constexpr auto value = true; };

template <bool C, typename = void> struct EnableIf;
template <typename Type> struct EnableIf<true, Type>{ using type = Type; };

template <typename Config, typename Transitions, typename = void>
struct TransitionFunction
{
    // @TODO: Implement this
    using end_input = typename Config::input_tape;
    using end_config = typename Config;
    using end_state = typename Config::state;
    static constexpr auto end_position = Config::position;
};

template <typename InputTape, typename State, int Position, typename Transitions>
struct TransitionFunction<Configuration<InputTape, State, Position>, Transitions,
                          typename EnableIf<is_same<State, QAccept>::value ||
                                            is_same<State, QReject>::value>::type>
{
    // @TODO: Implement this
    using end_config = Configuration<InputTape, State, Position>;
    using end_input = InputTape;
    using end_state = State;
    static constexpr auto end_position = Position;
};