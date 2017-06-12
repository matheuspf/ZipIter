/** 
 *  @file    ZipIter.h
 *  
 *  @brief Simple facilities to iterate through multiple containers
 *         and iterators at the same time, similar to Python's zip.
 *         Works easily with stl algorithms as well.
 */



#ifndef ZIP_ITER_H
#define ZIP_ITER_H

#include "Helpers.h"



namespace it
{


/** \class ZipIter
  *
  * Main iterator class. Inherits from std::iterator, being of the
  * most generic std::iterator_category from all of its arguments.
  * The value type is a tuple of the value_type's of all the arguments.
  * The basic iterator interface is implemented, while some functions
  * are only alowed if all the iterator parameters meet some requirements.
  * For example, the '+' and '-' operators are defined only for random
  * access operators.
*/

template <typename T, typename... Iters>
class ZipIter : public help::IteratorBase<T, std::remove_reference_t< Iters >...>
{
public:

        using Base = help::IteratorBase<T, std::remove_reference_t< Iters >...>;


        /// Some type definitions defined over the std::iterator base
        using iters_type = std::tuple<T, std::remove_reference_t< Iters >...>;

        using value_type      = typename Base::value_type;
        using reference       = typename Base::reference;
        using difference_type = typename Base::difference_type;

        using iterator_category = typename Base::iterator_category;



        /** A single constructor. Everything else is defaulted. The tuple
          * of iterators consists of values. No modification is done to 
          * the real iterators.
        */
        ZipIter (T t, Iters... iterators) : iters ( t, iterators... ) {}




        /** All the following operators simply apply a function to every member of the
          * tuple of iterators. Some of the methods are disabled if some iterator
          * does not meet all the requirements.
        */
        ZipIter& operator ++ () { help::execTuple(help::increment, iters); return *this; }

        ZipIter  operator ++ (int) { ZipIter temp{ *this }; operator++(); return temp; }


        template <class Tag = iterator_category, help::EnableIfMinimumTag< Tag, std::bidirectional_iterator_tag > = 0 >
        ZipIter& operator -- ()   { help::execTuple(help::decrement, iters); return *this; }

        template <class Tag = iterator_category, help::EnableIfMinimumTag< Tag, std::bidirectional_iterator_tag > = 0 >
        ZipIter operator -- (int) { ZipIter temp{ *this }; operator--(); return temp; }


        template <class Tag = iterator_category, help::EnableIfMinimumTag< Tag, std::random_access_iterator_tag > = 0 >
        ZipIter operator + (int inc) const { ZipIter temp{ *this }; help::execTuple(help::add, temp.iters, inc); return temp; }

        template <class Tag = iterator_category, help::EnableIfMinimumTag< Tag, std::random_access_iterator_tag > = 0 >
        ZipIter operator - (int inc) const { ZipIter temp{ *this }; help::execTuple(help::add, temp.iters, inc); return temp; }


        template <class Tag = iterator_category, help::EnableIfMinimumTag< Tag, std::random_access_iterator_tag > = 0 >
        difference_type operator + (const ZipIter& zp) const { return std::get<0>(iters) + std::get<0>(zp.iters); }

        template <class Tag = iterator_category, help::EnableIfMinimumTag< Tag, std::random_access_iterator_tag > = 0 >
        difference_type operator - (const ZipIter& zp) const { return std::get<0>(iters) - std::get<0>(zp.iters); }


        /// These are overloaded to also compare with a iterator of type T (the first of the tuple)
        template <typename Iter>
        bool operator < (const Iter& iter) const { return less(iter); }

        template <typename Iter>
        bool operator == (const Iter& iter) const { return equal(iter); }

        template <typename Iter>
        bool operator != (const Iter& iter) const { return !equal(iter); }



        /// Delegating
        decltype(auto) operator * () { return dereference( std::make_index_sequence< sizeof... (Iters) + 1 >() ); }

        decltype(auto) operator * () const { return dereference( std::make_index_sequence< sizeof... (Iters) + 1 >() ); }



private:



    /// Here the tuple of value is returned as a temporary to avoid any extra extorage or access
    template <std::size_t... Is>
    auto dereference (std::index_sequence<Is...>)
    {
        return std::forward_as_tuple( *std::get< Is >( iters )... );
    }

    template <std::size_t... Is>
    auto dereference (std::index_sequence<Is...>) const
    {
        return std::forward_as_tuple( *std::get< Is >( iters )... );
    }




    /// Comparisons. The first element defines the range
    bool less (const T& iter) const { return std::get<0>(iters) < iter; }

    bool less (const ZipIter& iter) const { return std::get<0>(iters) < std::get<0>(iter.iters); }


    bool equal (const T& iter) const { return std::get<0>(iters) == iter; }

    bool equal (const ZipIter& iter) const { return std::get<0>(iters) == std::get<0>(iter.iters); }



    /// The tuple of iterators
    iters_type iters;

};





/** \class Zip
  *
  * This class servers mainly as a wrapper for iterating in the for range loop.
  * It is composed of a tuple of references to containers that are iterable,
  * and exposes a 'begin' and 'end' methods, returning a 'ZipIter' with the
  * iterators of each container passed as argument.
*/
template <typename... Containers>
class Zip
{
public:


    /// Some type definitions
    using value_type = std::tuple < Containers... >;

    using iterator = ZipIter < typename help::Iterable<std::remove_reference_t<Containers>>::iterator... >;

    using iterator_category = typename iterator::iterator_category;

    using const_iterator = iterator;

    static constexpr std::size_t containersSize = sizeof... (Containers);


    /// Also a single constructor
    Zip (Containers... containers) : containers( containers... ) {}



    /// begin and end methods
    iterator begin () { return begin( std::make_index_sequence<containersSize>() ); }

    const_iterator begin () const { return begin( std::make_index_sequence<containersSize>() ); }


    iterator end () { return end( std::make_index_sequence<containersSize>() ); }

    const_iterator end () const { return end( std::make_index_sequence<containersSize>() ); }



    /// This is simply a facility for acessing random access containers, returning a tuple of values
    template <class Tag = iterator_category, help::EnableIfMinimumTag< Tag, std::random_access_iterator_tag > = 0 >
    auto operator [] (std::size_t pos)
    {
        return at( pos, std::make_index_sequence<containersSize>() );
    }

    template <class Tag = iterator_category, help::EnableIfMinimumTag< Tag, std::random_access_iterator_tag > = 0 >
    auto operator [] (std::size_t pos) const
    {
        return at( pos, std::make_index_sequence<containersSize>() );
    }



    /// The size of the first element defines the range
    std::size_t size () const { return std::get<0>( containers ).size(); }  



private:


    /// The actual implementations
    template <std::size_t... Is>
    iterator begin (std::index_sequence<Is...>)
    {
        return iterator( help::begin( std::get<Is>( containers ) )... );
    }

    template <std::size_t... Is>
    const_iterator begin (std::index_sequence<Is...>) const
    {
        return const_iterator( help::begin( std::get<Is>( containers ) )... );
    }

    template <std::size_t... Is>
    iterator end (std::index_sequence<Is...>)
    {
        return iterator( help::end( std::get<Is>( containers ) )... );
    }

    template <std::size_t... Is>
    const_iterator end (std::index_sequence<Is...>) const
    {
        return const_iterator( help::end( std::get<Is>( containers ) )... );
    }

    template <std::size_t... Is,
              class Tag = iterator_category,help::EnableIfMinimumTag< Tag, std::random_access_iterator_tag > = 0 >
    auto at (std::size_t pos, std::index_sequence<Is...>)
    {
        return std::forward_as_tuple( containers[ Is ]... );
    }

    template <std::size_t... Is,
              class Tag = iterator_category, help::EnableIfMinimumTag< Tag, std::random_access_iterator_tag > = 0 >
    auto at (std::size_t pos, std::index_sequence<Is...>) const
    {
        return std::forward_as_tuple( containers[ Is ]... );
    }



    /// Tuple of references to containers
    value_type containers;

};





/** These are the functions that will actually be called instead of
  * initializing the classes with cumbersome types. I used the first type
  * separatelly because it is easier to defined constraints (the first
  * element of a container cannot be a pointer) and forces the call with
  * at least 1 element
*/
template <typename T, typename... Iterators>
auto zipIter (T&& t, Iterators&&... iterators)
{
    return ZipIter<T, Iterators...>(std::forward<T>(t), std::forward<Iterators>(iterators)...);
}


template <typename T, typename... Containers, std::enable_if_t< !std::is_pointer< T >::value, int > = 0>
auto zip (T&& t, Containers&&... containers)
{
    return Zip<T, Containers...>(std::forward<T>(t), std::forward<Containers>(containers)...);
}




/** These are facilities for calling 'zipIter' more easily. You can simply
  * pass a container (or a pointer) with a defined 'std::begin' or 
  *'std::end' and it will call the proper function. The 'zipAll' returns
  * a std::pair containing both the begin and end of the iterators
*/
template <typename T, typename... Containers, std::enable_if_t< !std::is_pointer< T >::value, int > = 0>
auto zipBegin (T&& t, Containers&&... containers)
{
    return zipIter(help::begin(std::forward<T>(t)), help::begin(std::forward<Containers>(containers))...);
}

template <typename T, typename... Containers, std::enable_if_t< !std::is_pointer< T >::value, int > = 0>
auto zipEnd (T&& t, Containers&&... containers)
{
    return zipIter(help::end(std::forward<T>(t)), help::end(std::forward<Containers>(containers))...);
}

template <typename T, typename... Containers, std::enable_if_t< !std::is_pointer< T >::value, int > = 0>
auto zipAll (T&& t, Containers&&... containers)
{
    return std::make_pair(zipBegin(std::forward<T>(t), std::forward<Containers>(containers)...),
                          zipEnd  (std::forward<T>(t), std::forward<Containers>(containers)...));
}






/** \class UnZip
  *
  * This class has a single method that takes variadic arguments
  * including tuples and expand all of them, passing to a function.
  * This way you can handle the elements of a tuple separatelly.
  * It is very useful while iterating in the for range loops and in
  * stl functions. Please, see the folder examples for more.
*/
template <class Apply>
struct UnZip
{
    /// A single constructor takin a function as argument
    UnZip (const Apply& apply = Apply()) : apply(apply) {}



    /// These functions expand the arguments and aplly the function, using some helpers
    template <typename... Args>
    decltype(auto) operator () (Args&&... args)
    {
        return operator()( std::make_index_sequence< help::CountElements< std::decay_t< Args >... >::value >(),
                           std::forward< Args >( args )...);
    }

    template <std::size_t... Is, typename... Args>
    decltype(auto) operator () (std::index_sequence< Is... >, Args&&... args)
    {
        return apply( std::get< Is >( help::packArgs( std::forward< Args >( args )... ) )... );
    }


    /// The function to apply
    Apply apply;
};




/** As in the 'zipIter' and 'zip' cases, this function is much easier 
  * than to call than to instantiate the class. The first function gets
  * only a function as parameter, and is intended to use in stl functions.
  * The other two are for the for range loop, and gets a tuple as parameter
  * as well.
*/
template <class F>
auto unZip (F f)
{
    return UnZip<F>(f);
}

template <class Tuple, class Function, std::size_t... Is>
decltype(auto) unZip (Tuple&& tup, Function function, std::index_sequence<Is...>)
{
    return function( std::get< Is >( std::forward<Tuple>(tup) )... );
}

template <class Tuple, class Function>
decltype(auto) unZip (Tuple&& tup, Function function)
{
    return unZip(std::forward<Tuple>(tup), function, std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>());
}




/** This function makes a call to the for loop unpacking the parameters
  * with the 'unZip' function. A thing to notice is that the function
  * is actually the first parameter of the variadic arguments. The order
  * is changed with the 'help::reverse' function.
  */
template <typename... Args>
void forEach (Args&&... args)
{
    help::reverse<sizeof...(Args)-1>([](auto apply, auto&&... elems)
    {
        for(auto&& tup : zip(std::forward<decltype(elems)>(elems)...))
        {
            unZip(std::forward<decltype(tup)>(tup), apply);
        }

    }, std::forward<Args>(args)...);
}


} // namespace it




#endif	// ZIP_ITER_H
