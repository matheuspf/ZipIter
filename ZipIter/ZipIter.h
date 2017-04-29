/** 
 *  @file    ZipIter.h
 *  @author  Matheus Pedroza (matheuspf)
 *  @date    April 2017
 *  
 *  @brief Simple facilities to iterate through multiple
 *         containers at the same time similar to Python's zip
 * 
 *
 *  @section DESCRIPTION
 *  
 *   Given two iterable containers 'v' and 'u', a way in 
 *   the current C++11 to iterate between the two containers 
 *   at the same time is
 *
 *
 *
 *
 *
 */



#ifndef ZIP_ITER_H
#define ZIP_ITER_H

#include <type_traits>
#include <tuple>
#include <iterator>


#define NARGS_(_1,_2,_3,_4,_5,_6,_7,_8,N,...) N
#define NARGS(...) NARGS_(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)

#define EXPAND(...) __VA_ARGS__

#define CONCAT(x, y) CONCAT_(x, y)
#define CONCAT_(x, y) EXPAND(x ## y)


#define ZIP_BEGIN1(x, ...) std::begin(x)
#define ZIP_BEGIN2(x, ...) std::begin(x), ZIP_BEGIN1(__VA_ARGS__)
#define ZIP_BEGIN3(x, ...) std::begin(x), ZIP_BEGIN2(__VA_ARGS__)
#define ZIP_BEGIN4(x, ...) std::begin(x), ZIP_BEGIN3(__VA_ARGS__)
#define ZIP_BEGIN5(x, ...) std::begin(x), ZIP_BEGIN4(__VA_ARGS__)
#define ZIP_BEGIN6(x, ...) std::begin(x), ZIP_BEGIN5(__VA_ARGS__)
#define ZIP_BEGIN7(x, ...) std::begin(x), ZIP_BEGIN6(__VA_ARGS__)
#define ZIP_BEGIN8(x, ...) std::begin(x), ZIP_BEGIN7(__VA_ARGS__)

#define ZIP_END1(x, ...) std::end(x)
#define ZIP_END2(x, ...) std::end(x), ZIP_END1(__VA_ARGS__)
#define ZIP_END3(x, ...) std::end(x), ZIP_END2(__VA_ARGS__)
#define ZIP_END4(x, ...) std::end(x), ZIP_END3(__VA_ARGS__)
#define ZIP_END5(x, ...) std::end(x), ZIP_END4(__VA_ARGS__)
#define ZIP_END6(x, ...) std::end(x), ZIP_END5(__VA_ARGS__)
#define ZIP_END7(x, ...) std::end(x), ZIP_END6(__VA_ARGS__)
#define ZIP_END8(x, ...) std::end(x), ZIP_END7(__VA_ARGS__)


#define ZIP_ALL(...) EXPAND(it::zipIter(EXPAND(CONCAT(ZIP_BEGIN, NARGS(__VA_ARGS__)))(__VA_ARGS__)),  \
                            it::zipIter(EXPAND(CONCAT(ZIP_END,   NARGS(__VA_ARGS__)))(__VA_ARGS__)))




namespace it
{

namespace help
{

template <class Function, typename... Args, std::size_t... Is, typename... FuncArgs>
void execTuple (Function function, std::tuple<Args...>& tup, std::index_sequence<Is...>, FuncArgs&&... funcArgs)
{
    const auto& dummie = { ( function( std::get<Is>(tup), std::forward<FuncArgs>(funcArgs)... ), int{} ) ... };
}

template <class Function, typename... Args, typename... FuncArgs>
void execTuple (Function function, std::tuple<Args...>& tup, FuncArgs&&... funcArgs)
{
    return execTuple(function, tup, std::make_index_sequence<sizeof...(Args)>(), std::forward<FuncArgs>(funcArgs)...);
}



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



template <typename T>
struct Iterable { using iterator = typename T::iterator; };

template <typename T, std::size_t N>
struct Iterable<T[N]> { using iterator = T[N]; };

template <typename T>
struct Iterable<T*> { using iterator = T*; };





template <class> struct IterTagOrder;

template <>
struct IterTagOrder < std::forward_iterator_tag >
{
    static constexpr int value = 0;
};

template <>
struct IterTagOrder < std::bidirectional_iterator_tag >
{
    static constexpr int value = IterTagOrder < std::forward_iterator_tag >::value + 1;
};

template <>
struct IterTagOrder < std::random_access_iterator_tag >
{
    static constexpr int value = IterTagOrder < std::bidirectional_iterator_tag >::value + 1;
};

template <class T>
constexpr int IterTagOrder_v = IterTagOrder < T >::value;




template <class...>
struct SelectIterTag;

template <class Iter>
struct SelectIterTag < Iter >
{
    using type = Iter;
};

template <class Iter1, class Iter2>
struct SelectIterTag < Iter1, Iter2 >
{
    using type = std::conditional_t< ((IterTagOrder_v< Iter1 >) < (IterTagOrder_v< Iter2>)), Iter1, Iter2 >;
};

template <class Iter, class... Iters>
struct SelectIterTag < Iter, Iters... > : SelectIterTag< Iter, typename SelectIterTag< Iters... >::type > {};

template <class... Iter>
using SelectIterTag_t = typename SelectIterTag< Iter... >::type;




auto increment = [](auto&& x) { return ++x; };

auto decrement = [](auto&& x) { return --x; };

auto add       = [](auto&& x, int inc) { return x + inc; };


} // namespace help





template <class... Iters>
class ZipIter : std::iterator < help::SelectIterTag_t< typename std::iterator_traits< Iters >::iterator_category... >, 
                                    std::tuple< Iters... > >
{
public:

        using Base = std::iterator < help::SelectIterTag_t< typename std::iterator_traits< Iters >::iterator_category... >, 
                                     std::tuple< Iters... > >;

        using value_type      = typename Base::value_type;
        using reference       = typename Base::reference;
        using difference_type = typename Base::difference_type;

        using iterator_category = typename Base::iterator_category;


        ZipIter (Iters... iterators) : iters ( iterators... ) {}

        ZipIter (std::tuple<Iters...> iters) : iters( std::move( iters ) ) {}



        ZipIter& operator ++ () { help::execTuple(help::increment, iters); return *this; }

        ZipIter  operator ++ (int) { ZipIter temp{ *this }; help::execTuple(help::increment, iters); return temp; }


        template <class Tag = iterator_category, 
                  std::enable_if_t< std::is_same< Tag, std::bidirectional_iterator_tag >::value, int > = 0 >
        ZipIter& operator -- ()   { help::execTuple(help::decrement, iters); return *this; }

        template <class Tag = iterator_category, 
                  std::enable_if_t< std::is_same< Tag, std::bidirectional_iterator_tag >::value, int > = 0 >
        ZipIter operator -- (int) { ZipIter temp{ *this }; help::execTuple(help::decrement, iters); return temp; }


        template <class Tag = iterator_category, 
                  std::enable_if_t< std::is_same< Tag, std::random_access_iterator_tag >::value, int > = 0 >
        ZipIter operator + (int inc) const { ZipIter temp{ *this }; help::execTuple(help::add, temp.iters, inc); return temp; }

        template <class Tag = iterator_category, 
                  std::enable_if_t< std::is_same< Tag, std::random_access_iterator_tag >::value, int > = 0 >
        ZipIter operator - (int inc) const { ZipIter temp{ *this }; help::execTuple(help::add, temp.iters, inc); return temp; }


        difference_type operator + (const ZipIter& zp) const { return std::get<0>(iters) + std::get<0>(zp.iters); }

        difference_type operator - (const ZipIter& zp) const { return std::get<0>(iters) - std::get<0>(zp.iters); }


        template <typename Iter>
        bool operator == (const Iter& iter) const { return equal(iter); }

        template <typename Iter>
        bool operator != (const Iter& iter) const { return !equal(iter); }


        decltype(auto) operator * () { return dereference( std::make_index_sequence< sizeof... (Iters) >() ); }

        decltype(auto) operator * () const { return dereference( std::make_index_sequence< sizeof... (Iters) >() ); }



private:


    template <std::size_t... Is>
    decltype(auto) dereference (std::index_sequence<Is...>)
    {
        return std::forward_as_tuple( *std::get< Is >( iters )... );
    }

    template <std::size_t... Is>
    decltype(auto) dereference (std::index_sequence<Is...>) const
    {
        return std::forward_as_tuple( *std::get< Is >( iters )... );
    }



    template <typename Iter>
    bool equal (const Iter& iter) const { return std::get<0>(iters) == iter; }

    bool equal (const ZipIter& zipIter) const { return std::get<0>(iters) == std::get<0>(zipIter.iters); }



    value_type iters;

};


template <class... Containers>
class Zip
{
public:

    using value_type = std::tuple < Containers... >;

    using iterator = ZipIter < typename help::Iterable< std::decay_t< Containers > >::iterator... >;

    using const_iterator = iterator;

    static constexpr std::size_t containersSize = sizeof... (Containers);



    Zip (Containers... conts) : containers( conts... ) {}



    iterator begin () { return begin( std::make_index_sequence<containersSize>() ); }

    const_iterator begin () const { return begin( std::make_index_sequence<containersSize>() ); }


    iterator end () { return end( std::make_index_sequence<containersSize>() ); }

    const_iterator end () const { return end( std::make_index_sequence<containersSize>() ); }


    template <class Tag = typename iterator::iterator_category,
              std::enable_if_t< std::is_same < Tag, std::random_access_iterator_tag >::value, int > = 0>
    decltype(auto) operator [] (std::size_t pos)
    {
        return at( pos, std::make_index_sequence<containersSize>() );
    }

    template <class Tag = typename iterator::iterator_category,
              std::enable_if_t< std::is_same < Tag, std::random_access_iterator_tag >::value, int > = 0>
    decltype(auto) operator [] (std::size_t pos) const
    {
        return at( pos, std::make_index_sequence<containersSize>() );
    }


    std::size_t size () const { return std::get<0>( containers ).size(); }  



private:

    template <std::size_t... Is>
    iterator begin (std::index_sequence<Is...>)
    {
        return iterator( std::begin( std::get<Is>( containers ) )... );
    }

    template <std::size_t... Is>
    const_iterator begin (std::index_sequence<Is...>) const
    {
        return const_iterator( std::begin( std::get<Is>( containers ) )... );
    }


    template <std::size_t... Is>
    iterator end (std::index_sequence<Is...>)
    {
        return iterator( std::end( std::get<Is>( containers ) )... );
    }

    template <std::size_t... Is>
    const_iterator end (std::index_sequence<Is...>) const
    {
        return const_iterator( std::end( std::get<Is>( containers ) )... );
    }


    template <class Tag = typename iterator::iterator_category, std::size_t... Is,
              std::enable_if_t< std::is_same < Tag, std::random_access_iterator_tag >::value, int > = 0>
    decltype(auto) at (std::size_t pos, std::index_sequence<Is...>)
    {
        return std::forward_as_tuple( containers[ Is ]... );
    }

    template <class Tag = typename iterator::iterator_category, std::size_t... Is,
              std::enable_if_t< std::is_same < Tag, std::random_access_iterator_tag >::value, int > = 0>
    decltype(auto) at (std::size_t pos, std::index_sequence<Is...>) const
    {
        return std::forward_as_tuple( containers[ Is ]... );
    }



    value_type containers;

};





template <class... Containers>
auto zip (Containers&&... containers)
{
    return Zip<Containers...>(std::forward<Containers>(containers)...);
}


template <typename... Iterators>
auto zipIter (Iterators&&... iterators)
{
    return ZipIter<Iterators...>(std::forward<Iterators>(iterators)...);
}





template <class F>
struct UnZip
{
    UnZip (const F& f = F()) : f(f) {}

    template <typename... Args, typename Indexes = std::make_index_sequence<sizeof...(Args)>>
    void operator () (std::tuple<Args...>&& tup) { unpack(tup, Indexes()); }

    template <typename... Args, std::size_t... Is>
    void unpack (std::tuple<Args...>& tup, std::index_sequence<Is...>) { f(std::get<Is>(tup)...); }


    F f;
};



template <class F>
auto unZip (F f)
{
    return UnZip<F>(f);
}



template <class  Tuple, class Function, std::size_t... Is>
void unZip (Tuple&& tup, Function function, std::index_sequence<Is...>)
{
    function( std::get< Is >( std::forward<Tuple>(tup) )... );
}

template <class Tuple, class Function>
void unZip (Tuple&& tup, Function function)
{
    return unZip(std::forward<Tuple>(tup), function, std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>());
}




template <typename... Args>
void forEach (Args&&... args)
{
    static auto exec = [](auto f, auto&&... elems)
    { 
        for(auto&& tup : zip(std::forward<decltype(elems)>(elems)...))
            unZip(std::forward<decltype(tup)>(tup), f);
    };

    help::reverse<sizeof...(Args)-1>(exec, std::forward<Args>(args)...);
}


} // namespace it


#endif	// ZIP_ITER_H
