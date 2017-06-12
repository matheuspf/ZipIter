/** 
 *  \file Helpers.h  
 *  \brief Some metaprogramming helpers
 */

#ifndef HELPERS_ZIP_ITER_H
#define HELPERS_ZIP_ITER_H

#include <type_traits>
#include <tuple>
#include <iterator>


/// Trick to get the number of arguments passed to a macro
#define NARGS_(_1, _2, _3, _4, _5, _6, _7, _8, N,...) N
#define NARGS(...) NARGS_(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)


/// Makes easier to expand the expressions
#define EXPAND(...) __VA_ARGS__

/// Concatenate two tokens
#define CONCAT(x, y) CONCAT_(x, y)
#define CONCAT_(x, y) EXPAND(x ## y)


/** When using stl functions which takes iterator parameters of the 
  * form (first, last), these macros make it much easier. Simply use
  * ZIP_ALL(container) for a container class that is iterable (that is,
  * has std::begin and std::end definitions or is a pointer).
  * These macros simply call it::help::begin and it::help::end for
  * each argument, delegating the call to std::begin and std::end
  * while also supporting pointer types to be called.
*/
#define ZIP_BEGIN1(x, ...) ::it::help::begin(x)
#define ZIP_BEGIN2(x, ...) ::it::help::begin(x), ZIP_BEGIN1(__VA_ARGS__)
#define ZIP_BEGIN3(x, ...) ::it::help::begin(x), ZIP_BEGIN2(__VA_ARGS__)
#define ZIP_BEGIN4(x, ...) ::it::help::begin(x), ZIP_BEGIN3(__VA_ARGS__)
#define ZIP_BEGIN5(x, ...) ::it::help::begin(x), ZIP_BEGIN4(__VA_ARGS__)
#define ZIP_BEGIN6(x, ...) ::it::help::begin(x), ZIP_BEGIN5(__VA_ARGS__)
#define ZIP_BEGIN7(x, ...) ::it::help::begin(x), ZIP_BEGIN6(__VA_ARGS__)
#define ZIP_BEGIN8(x, ...) ::it::help::begin(x), ZIP_BEGIN7(__VA_ARGS__)

#define ZIP_END1(x, ...) ::it::help::end(x)
#define ZIP_END2(x, ...) ::it::help::end(x), ZIP_END1(__VA_ARGS__)
#define ZIP_END3(x, ...) ::it::help::end(x), ZIP_END2(__VA_ARGS__)
#define ZIP_END4(x, ...) ::it::help::end(x), ZIP_END3(__VA_ARGS__)
#define ZIP_END5(x, ...) ::it::help::end(x), ZIP_END4(__VA_ARGS__)
#define ZIP_END6(x, ...) ::it::help::end(x), ZIP_END5(__VA_ARGS__)
#define ZIP_END7(x, ...) ::it::help::end(x), ZIP_END6(__VA_ARGS__)
#define ZIP_END8(x, ...) ::it::help::end(x), ZIP_END7(__VA_ARGS__)

#define ZIP_ALL(...) EXPAND(::it::zipIter(EXPAND(CONCAT(ZIP_BEGIN, NARGS(__VA_ARGS__)))(__VA_ARGS__)),  \
                            ::it::zipIter(EXPAND(CONCAT(ZIP_END,   NARGS(__VA_ARGS__)))(__VA_ARGS__)))





namespace std
{

    /** This call converts a rvalue tuple to a lvalue and calls std::swap.
      * This definition is necessary for some stl functions that use 
      * std::swap. As long as I could see, there is no performance overhead
    */
    template <typename... Args>
    void swap (std::tuple<Args...>&& a, std::tuple<Args...>&& b) noexcept
    {
        swap(a, b);
    }
}



/// Main namespace
namespace it
{

// Helper namespace
namespace help
{

/**
  * \brief This function applies a function to every argument of the tuple
  * \param apply The function to be applied
  * \param tup A std::tuple
  * \param std::index_sequence Necessary for expanding the tuple
  * \param funcArgs Extra arguments of the function
  */
template <class Apply, typename... Args, std::size_t... Is, typename... FuncArgs>
void execTuple (Apply apply, std::tuple<Args...>& tup, std::index_sequence<Is...>, FuncArgs&&... funcArgs)
{
    const auto& dummie = { ( apply( std::get<Is>(tup), std::forward<FuncArgs>(funcArgs)... ), int{} ) ... };
}

template <class Apply, typename... Args, typename... FuncArgs>
void execTuple (Apply apply, std::tuple<Args...>& tup, FuncArgs&&... funcArgs)
{
    return execTuple(apply, tup, std::make_index_sequence<sizeof...(Args)>(), std::forward<FuncArgs>(funcArgs)...);
}




/** Given an index P, this function reverses the order
  * of the arguments and apply a function to every element
  * in the new order. Serves only as syntatic sugar.
  */
template <std::size_t P, class Apply, class... Args, std::size_t... Is, std::size_t... Js>
decltype(auto) reverse (Apply apply, std::tuple<Args...>&& tup, std::index_sequence<Is...>, std::index_sequence<Js...>)
{
    return apply(std::get<Js+P>(tup)..., std::get<Is>(tup)...);
}

template <std::size_t P, class Apply, class... Args>
decltype(auto) reverse (Apply apply, Args&&... args)
{
    return reverse<P>(apply, std::forward_as_tuple(std::forward<Args>(args)...), 
                          std::make_index_sequence<P>{}, 
                          std::make_index_sequence<sizeof...(Args)-P>{});
}


/// Simple trait for defining iterable types
template <typename T>                struct Iterable             { using iterator = typename T::iterator; };
template <typename T>                struct Iterable<const T>    { using iterator = typename T::const_iterator; };
template <typename T, std::size_t N> struct Iterable<T[N]>       { using iterator = T*; };
template <typename T, std::size_t N> struct Iterable<const T[N]> { using iterator = const T*; };
template <typename T>                struct Iterable<T*>         { using iterator = T*; };
template <typename T>                struct Iterable<const T*>   { using iterator = const T*; };



/// This is the order of the iterator types. The smaller is the more generic.
constexpr int iterTagOrder ( std::forward_iterator_tag )       { return 0; }
constexpr int iterTagOrder ( std::bidirectional_iterator_tag ) { return 1; }
constexpr int iterTagOrder ( std::random_access_iterator_tag ) { return 2; }



/// Selects the smallest (the more generic) iterator type from an variadic argument list
template <class...>   struct SelectIterTag;

template <class Iter> struct SelectIterTag < Iter > { using type = Iter; };

template <class Iter1, class Iter2>
struct SelectIterTag < Iter1, Iter2 >
{
    using type = std::conditional_t< (iterTagOrder(Iter1{}) < iterTagOrder(Iter2{})), Iter1, Iter2 >;
};

template <class Iter, class... Iters>
struct SelectIterTag < Iter, Iters... > : SelectIterTag< Iter, typename SelectIterTag< Iters... >::type > {};

template <class... Iter> using SelectIterTag_t = typename SelectIterTag< Iter... >::type;




// Avoids some boilerplate in the definition of the 'ZipIter' class
template <typename... Iters>
using IteratorBase = std::iterator < help::SelectIterTag_t< typename std::iterator_traits< Iters >::iterator_category... >, 
                                     std::tuple< typename std::iterator_traits< Iters >::value_type... > >;



/// Only let functions to be called if the iterator tag satisfies a minimum tag
template <typename Tag, typename MinimumTag>
using EnableIfMinimumTag = std::enable_if_t< iterTagOrder( Tag{} ) >= iterTagOrder( MinimumTag {} ), int >; 








/// Simple functions to be used in the 'execTuple' function. Defined as lambdas for simplicity.
auto increment = [](auto&& x) { return ++x; };

auto decrement = [](auto&& x) { return --x; };

auto add       = [](auto&& x, int inc) { return x = x + inc; };




/** The sole reason these functions were defined is to allow pointers 
  * to be called in 'zip' as if they were iterable types, having a starting and
  * ending points. As the first argument defines the range, there is no
  * problem in passing a pointer to be iterated, as long as its range
  * is smaller than the range of the first argument of the 'zip' function.
*/
template <typename T, std::enable_if_t< !std::is_pointer< std::decay_t<T> >::value, int > = 0>
decltype(auto) begin (T&& t) noexcept
{
    return std::begin(std::forward<T>(t)); /// Simply delegating the call for any other type
}

template <typename T, std::enable_if_t< std::is_pointer< std::decay_t<T> >::value, int > = 0>
decltype(auto) begin (T t) noexcept
{
    return t; /// The pointer itself is the starting point
}


template <typename T, std::enable_if_t< !std::is_pointer< std::decay_t<T> >::value, int > = 0>
decltype(auto) end (T&& t) noexcept
{
    return std::end(std::forward<T>(t)); /// Simply delegating the call for any other type
}

template <typename T, std::enable_if_t< std::is_pointer< std::decay_t<T> >::value, int > = 0>
decltype(auto) end (T t) noexcept
{
    return t + std::size_t(1e13); /// It is simply a unreachable point
}




/** This utility takes variadic arguments and packs them in a single tuple.
  * If any of the arguments itself is a tuple, it is concatenated (via std::tuple_cat)
  * with the rest of the arguments, always resulting in a single tuple.
*/
template <typename... ArgsTup>
decltype(auto) packArgs (std::tuple< ArgsTup... > tup)
{
    return std::move( tup );
}

template <typename T>
decltype(auto) packArgs (T&& t)
{
    return std::forward_as_tuple( std::forward< T >( t ) );
}

template <typename... ArgsTup, typename... Args>
decltype(auto) packArgs (std::tuple<ArgsTup...> tup, Args&&... args)
{
    return std::tuple_cat( std::move( tup ), std::move( packArgs(std::forward<Args>(args)...) ) );
}

template <typename T, typename... Args>
decltype(auto) packArgs (T&& t, Args&&... args)
{
    return std::tuple_cat( std::forward_as_tuple( std::forward< T >( t ) ),
                           std::move( packArgs(std::forward<Args>(args)...) ) );
}





/// Counts the number of arguments. If an argument is a tuple, sums its total size.
template <std::size_t V> struct st_constant : std::integral_constant< std::size_t, V > {};

template <typename...> struct CountElements;

template <> struct CountElements <> : st_constant< 0 > { };

template <typename T, typename... Args>
struct CountElements< T, Args... > : st_constant< 1 + CountElements< Args... >::value > {};

template <typename... TupArgs, typename... Args>
struct CountElements< std::tuple< TupArgs... >, Args... > : st_constant< sizeof...(TupArgs) + 
                                                                         CountElements< Args... >::value > {};



} // namespace help

} // namespace it


#endif // HELPERS_ZIP_ITER_H