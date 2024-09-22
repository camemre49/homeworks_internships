
#pragma once
#include "typelist.h"
#include <type_traits>

template<typename ...Ts>
struct Size<List<Ts...>> {
    static constexpr auto value = sizeof...(Ts);
};

// @brief base case -- Get the 0th element of a list
template<typename T, typename... Ts>
struct At<0, List<T, Ts...>> {
    using type = T;
};

// @brief inductive case -- Get the Nth element of a list
template<int index, typename T, typename... Ts>
struct At<index, List<T, Ts...>> {
    static_assert(index > 0, "index cannot be negative");
    using type = typename At<index - 1, List<Ts...>>::type;
};

struct NotImpl;
#define NOT_IMPL NotImpl

/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * Implementation of Find  * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
template<typename Q, int index>
struct Find<Q, index, List<>> {
    static constexpr auto value = -1;
};

template<typename Q, int index, typename... Ts>
struct Find<Q, index, List<Q, Ts...>> {
    static constexpr int value = index;
};

template<typename Q, int index, typename T, typename... Ts>
struct Find<Q, index, List<T, Ts...>> {
    static constexpr int value = Find<Q, index + 1, List<Ts...>>::value;
};

/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * Implementation of Replace  * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

template<typename Q, typename T, typename... Ts>
struct Replace<Q, 0, List<T, Ts...>> {
    using type = List<Q, Ts...>;
};

template<typename Q, int index, typename T, typename... Ts>
struct Replace<Q, index, List<T, Ts...>> {
    using type = typename Prepend<T, typename Replace<Q, index - 1, List<Ts...>>::type>::type;
};


/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * Implementation of Append  * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
template<typename NewItem>
struct Append<NewItem, List<>> {
    using type = List<NewItem>;
};

template<typename NewItem, typename... Ts>
struct Append<NewItem, List<Ts...>> {
    using type = List<Ts..., NewItem>;
};

/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * Implementation of Prepend  * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
template<typename NewItem, typename... Ts>
struct Prepend<NewItem, List<Ts...>> {
    using type = List<NewItem, Ts...>;
};

/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * Implementation of Map  * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
template<template<typename> typename F, typename Ret>
struct Map<F, Ret, List<>> {
    using type = Ret;
};

template<template<typename> typename F, typename... Ret, typename T, typename... Ts>
struct Map<F, List<Ret...>, List<T, Ts...>> {
    using type = typename Map<F, List<Ret..., typename F<T>::type>, List<Ts...>>::type;
};

/**
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * Implementation of Filter  * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
template<bool Cond, typename TrueType, typename FalseType>
struct Conditional;

template<typename TrueType, typename FalseType>
struct Conditional<true, TrueType, FalseType> {
    using type = TrueType;
};

template<typename TrueType, typename FalseType>
struct Conditional<false, TrueType, FalseType> {
    using type = FalseType;
};

template<bool Cond, typename TrueType, typename FalseType>
using conditional = typename Conditional<Cond, TrueType, FalseType>::type;

template<template<typename> typename F, typename Ret>
struct Filter<F, Ret, List<>> {
    using type = Ret;
};

template<template<typename> typename F, typename... Ret, typename T, typename... Ts>
struct Filter<F, List<Ret...>, List<T, Ts...>> {
    using type = typename conditional<
            F<T>::value,
            Filter<F, List<Ret..., T>, List<Ts...>>,
            Filter<F, List<Ret...>, List<Ts...>>
    >::type;
};