#ifndef ZIP_ITERATOR
#define ZIP_ITERATOR

#include <type_traits>
#include <tuple>
#include <iterator>


#define NARGS_(_1,_2,_3,_4,_5,_6,_7,_8,N,...) N
#define NARGS(...) NARGS_(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)

#define EXPAND(...) __VA_ARGS__

#define CONCAT(x, y) CONCAT_(x, y)
#define CONCAT_(x, y) EXPAND(x ## y)


#define ZIP_BEGIN1(x, ...) x.begin()
#define ZIP_BEGIN2(x, ...) x.begin(), ZIP_BEGIN1(__VA_ARGS__)
#define ZIP_BEGIN3(x, ...) x.begin(), ZIP_BEGIN2(__VA_ARGS__)
#define ZIP_BEGIN4(x, ...) x.begin(), ZIP_BEGIN3(__VA_ARGS__)
#define ZIP_BEGIN5(x, ...) x.begin(), ZIP_BEGIN4(__VA_ARGS__)
#define ZIP_BEGIN6(x, ...) x.begin(), ZIP_BEGIN5(__VA_ARGS__)
#define ZIP_BEGIN7(x, ...) x.begin(), ZIP_BEGIN6(__VA_ARGS__)
#define ZIP_BEGIN8(x, ...) x.begin(), ZIP_BEGIN7(__VA_ARGS__)

#define ZIP_END1(x, ...) x.end()
#define ZIP_END2(x, ...) x.end(), ZIP_END1(__VA_ARGS__)
#define ZIP_END3(x, ...) x.end(), ZIP_END2(__VA_ARGS__)
#define ZIP_END4(x, ...) x.end(), ZIP_END3(__VA_ARGS__)
#define ZIP_END5(x, ...) x.end(), ZIP_END4(__VA_ARGS__)
#define ZIP_END6(x, ...) x.end(), ZIP_END5(__VA_ARGS__)
#define ZIP_END7(x, ...) x.end(), ZIP_END6(__VA_ARGS__)
#define ZIP_END8(x, ...) x.end(), ZIP_END7(__VA_ARGS__)


#define ZIP_ALL(...) EXPAND(it::zip(EXPAND(CONCAT(ZIP_BEGIN, NARGS(__VA_ARGS__)))(__VA_ARGS__)),  \
                            it::zip(EXPAND(CONCAT(ZIP_END,   NARGS(__VA_ARGS__)))(__VA_ARGS__)))




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
struct SelectIterTag < Iter, Iters... >
{
    using type = typename SelectIterTag< Iter, typename SelectIterTag< Iters... >::type >::type;
};

template <class... Iter>
using SelectIterTag_t = typename SelectIterTag< Iter... >::type;




auto increment = [](auto&& x) { return ++x; };

auto decrement = [](auto&& x) { return --x; };

auto add       = [](auto&& x, int inc) { return x + inc; };


} // namespace help





template <class... Iters>
class ZipIterator : std::iterator < help::SelectIterTag_t< typename std::iterator_traits< Iters >::iterator_category... >, 
                                    std::tuple< Iters... > >
{
public:

        using Base = std::iterator < help::SelectIterTag_t< typename std::iterator_traits< Iters >::iterator_category... >, 
                                     std::tuple< Iters... > >;

        using value_type = typename Base::value_type;
        using reference  = typename Base::reference;
        using pointer    = typename Base::pointer;

        using iterator_category = typename Base::iterator_category;



        ZipIterator (Iters... iterators) : iters ( iterators... ) {}

        ZipIterator (std::tuple<Iters...> iters) : iters( std::move( iters ) ) {}



        ZipIterator& operator ++ () { help::execTuple(help::increment, iters); return *this; }

        ZipIterator  operator ++ (int) { ZipIterator temp{ *this }; help::execTuple(help::increment, iters); return temp; }


        template <class Tag = iterator_category, 
                  std::enable_if_t< std::is_same< Tag, std::bidirectional_iterator_tag >::value, int > = 0 >
        ZipIterator& operator -- ()   { help::execTuple(help::decrement, iters); return *this; }

        template <class Tag = iterator_category, 
                  std::enable_if_t< std::is_same< Tag, std::bidirectional_iterator_tag >::value, int > = 0 >
        ZipIterator operator -- (int) { ZipIterator temp{ *this }; help::execTuple(help::decrement, iters); return temp; }


        // template <class Tag = iterator_category, 
        //           std::enable_if_t< std::is_same< Tag, std::random_access_iterator_tag >::value, int > = 0 >
        // ZipIterator operator + (int inc) { ZipIterator temp{ *this }; help::execTuple(help::add, temp.iters, inc); return temp; }

        // template <class Tag = iterator_category, 
        //           std::enable_if_t< std::is_same< Tag, std::random_access_iterator_tag >::value, int > = 0 >
        // ZipIterator operator - (int inc) { ZipIterator temp{ *this }; help::execTuple(help::add, temp.iters, inc); return temp; }



        template <typename Iter>
        bool operator == (const Iter& iter) { return equal(iter); }

        template <typename Iter>
        bool operator != (const Iter& iter) { return !equal(iter); }


        decltype(auto) operator * () { return dereference( std::make_index_sequence< sizeof... (Iters) >() ); }



private:


    template <std::size_t... Is>
    decltype(auto) dereference (std::index_sequence<Is...>)
    {
        return std::make_tuple( *std::get< Is >( iters )... );
    }



    template <typename Iter>
    bool equal (const Iter& iter) { return std::get<0>(iters) == iter; }

    bool equal (const ZipIterator& zipIter) { return std::get<0>(iters) == std::get<0>(zipIter.iters); }



    value_type iters;

};





template <class... Containers>
class Zip
{
public:

    using value_type = std::tuple < std::decay_t< Containers >&... >;

    using iterator = ZipIterator < typename std::decay_t< Containers >::iterator... >;

    using const_iterator = iterator;

    static constexpr std::size_t containersSize = sizeof... (Containers);



    Zip ( Containers&... conts ) : containers( conts... ) {}



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
        return std::make_tuple( containers[ Is ]... );
    }

    template <class Tag = typename iterator::iterator_category, std::size_t... Is,
              std::enable_if_t< std::is_same < Tag, std::random_access_iterator_tag >::value, int > = 0>
    decltype(auto) at (std::size_t pos, std::index_sequence<Is...>) const
    {
        return std::make_tuple( containers[ Is ]... );
    }



    value_type containers;

};



// template <class F>
// struct UnZip
// {
//     UnZip (const F& f = F()) : f(f) {}

//     template <typename... Args, typename Indexes = std::make_index_sequence<sizeof...(Args)>>
//     void operator () (std::tuple<Args...>&& tup) { unpack(tup, Indexes()); }

//     template <typename... Args, std::size_t... Is>
//     void unpack (std::tuple<Args...>& tup, std::index_sequence<Is...>) { f(std::get<Is>(tup)...); }


//     F f;
// };





template <class... Containers>
auto zip (Containers&&... containers)
{
    return Zip<Containers...>(std::forward<Containers>(containers)...);
}


template <typename... Iterators>
auto zipIterator (Iterators&&... iterators)
{
    return ZipIterator<Iterators...>(std::forward<Iterators>(iterators)...);
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



template <typename... Args, class Function, std::size_t... Is>
void unZip (std::tuple<Args...>& tup, Function function, std::index_sequence<Is...>)
{
    function( std::get< Is >( tup )... );
}


template <typename... Args, class Function>
void unZip (std::tuple<Args...>& tup, Function function)
{
    return unZip(tup, function, std::make_index_sequence<sizeof...(Args)>());
}



template <class... Containers, class Function>
void forEach (Zip<Containers...>& zip_, Function function)
{
    for(auto&& tup : zip_) unZip(tup, function);
}

template <class... Containers, class Function>
void forEach (Zip<Containers...>&& zip_, Function function)
{
    for(auto&& tup : zip_) unZip(tup, function);
}

template <class... Containers, class Function>
void forEach (const Zip<Containers...>& zip_, Function function)
{
    for(auto&& tup : zip_) unZip(tup, function);
}




} // namespace it


#endif	// ZIP_ITERATOR
