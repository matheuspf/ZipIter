/** 
  *  \file LoopingExample.cpp
  *  
  * Here are some examples showing how to use the 'zip' and
  * 'zipIter' facilities to iterate simultaneously through
  * multiple containers.
  *
  * Something important. You really don't want to take a referrence
  * to the tuple returned by 'zip' or 'zipIter'. It is a temporary,
  * a rvalue referrence to a tuple of referrences of the values returned
  * by derreferencing each iterator. So even if you want to modify the 
  * values inside the loop, you can take the tuple as a copy. Taking it
  * as a lvalue referrence will generate a compile error.
*/ 




#include <bits/stdc++.h>

#include "ZipIter/ZipIter.h"


using namespace std;
using namespace it;



/// Setting the random number generator
mt19937 rng(random_device{}());

int rngInt(int l, int u)
{
  return uniform_int_distribution<>(l, u)(rng);
}



/// Some containers to iterate
vector<int> v;

set<short> s;

list<long> l;

unsigned* a;




/// Do something within the loop
auto func = [](auto x, auto y, auto w, auto z){ return x + 2 * y + 3 * w + 4 * z; };



/// To verify the output of 11 different types of iteration
vector<int> output(11, 0);




/// Random size (same size for all containers) and random values
void randomInit ()
{
    int n = rngInt(10, 20);

    a = new unsigned[n];

    for(int i = 0; i < n; ++i)
    {
        v.push_back(rngInt(0, 10));
        s.insert(rngInt(0, 10));
        l.push_back(rngInt(0, 10));
        a[i] = rngInt(0, 10);
    }
}



/** This is the usual way to iterate through all containers at 
  * the same tyme. As not all of them are random accesss iterators,
  * a simple for loop is not possible.
*/
void defaultExample ()
{
    auto itv = v.begin();
    auto its = s.begin();
    auto itl = l.begin();
    auto ita = a;
    auto ite = v.end();

    for(; itv != ite; ++itv, ++its, ++itl, ++ita)
        output[0] += func(*itv, *its, *itl, *ita);
}




/** This is how to iterate using the std::for_each. All the examples
  * below are almost the same, changing only the way in which we
  * acess the variables.
*/
void forEachExamples ()
{
    /** Passing both 'begin' and 'end' to 'zipIter'. Notice here that
      * I don't use a proper 'end' for the pointer 'a'. The first
      * container always defined the range, so we can use anithing.
      * The reason we have to pass a whole new 'zipIter' to for_each is
      * that the initial and final points have to be of the same type.
    */
    for_each(zipIter(v.begin(), s.begin(), l.begin(), &a[0]), 
             zipIter(v.end(), s.end(), l.end(), &a[0]), [](auto tup)
    {
        output[1] += func(get<0>(tup), get<1>(tup), get<2>(tup), get<3>(tup));
    });



    /// Using 'zipBegin' and 'zipEnd'
    for_each(zipBegin(v, s, l, a), zipEnd(v, s, l, a), [](auto tup)
    {
        output[2] += func(get<0>(tup), get<1>(tup), get<2>(tup), get<3>(tup));
        //cout << get<0>(tup) << "    " << get<1>(tup) << "   " << get<2>(tup) << "    " << get<3>(tup) << "\n";
    });



    /// Using 'zipAll' (returns a 'std::pair' containing 'begin' and 'end')
    auto range = zipAll(v, s, l, a);

    for_each(range.first, range.second, [](auto tup)
    {
        output[3] += func(get<0>(tup), get<1>(tup), get<2>(tup), get<3>(tup));
    });



    /// Using the macro helper 'ZIP_ALL' (it simply calls the first type)
    for_each(ZIP_ALL(v, s, l, a), [](auto tup)
    {
        output[4] += func(get<0>(tup), get<1>(tup), get<2>(tup), get<3>(tup));
    });




    /** The examples below are the same as above, but now using the
      * 'unZip' function to unpack the tuple returned by derreferencing
      * the iterators. It makes the access to individual elements much easier
    */
    for_each(zipIter(v.begin(), s.begin(), l.begin(), a), 
             zipIter(v.end(), s.end(), l.end(), a), unZip([](auto a, auto b, auto c, auto d)
    {
        output[5] += func(a, b, c, d);
    }));



    for_each(zipBegin(v, s, l, a), zipEnd(v, s, l, a), unZip([](auto a, auto b, auto c, auto d)
    {
        output[6] += func(a, b, c, d);
    }));



    range = zipAll(v, s, l, a);

    for_each(range.first, range.second, unZip([](auto a, auto b, auto c, auto d)
    {
        output[7] += func(a, b, c, d);
    }));



    for_each(ZIP_ALL(v, s, l, a), unZip([](auto a, auto b, auto c, auto d)
    {
        output[8] += func(a, b, c, d);
    }));
}




/** Here are the examples showing how to use the for range loop.
  * If you want to modify an element inside the loop using 'unZip',
  * take that element as a referrence, but always take the tuple
  * as a copy or rvalue referrence.
*/
void forRangeExamples ()
{
    /// Using a regular for range loop taking a tuple
    for(auto tup : zip(v, s, l, a))
        output[9] += func(get<0>(tup), get<1>(tup), get<2>(tup), get<3>(tup));


    /** Same as above, but now passing the tuple to 'unZip' to
      * unpack it. Notice here that we pass the tuple as first
      * argument. The only drawback is that we can't use 'break'
      * or 'continue' inside the lambda.
    */
    for(auto tup : zip(v, s, l, a)) unZip(tup, [](auto a, auto b, auto c, auto d)
    {
        output[10] += func(a, b, c, d);
    });




    /** This function only encapsulates the above example.
      * It is called forEach because as in 'std::forEach'
      * the function is applyed to every element, without permiting
      * 'break' or 'continue' statements. Notice that the lambda
      * is acctually the last element of the variadic arguments;
    */
      forEach(v, s, l, a, [](auto a, auto b, auto c, auto d)
      {
        output[11] += func(a, b, c, d);
      });
}









int main ()
{
    /// It is expected that the values of all loops are the same
    randomInit();

    defaultExample();

    forEachExamples();

    forRangeExamples();



    auto filled = [](int sz){ vector<int> v(sz); iota(v.begin(), v.end(), 0); return v; };


    /// A simple enumerate
    for(auto tup : zip(output, filled(output.size()))) unZip(tup, [](int x, int i)
    {
      cout << i << "     " << x << "\n";
    });



    assert(all_of(output.begin() + 1, output.end(), [](int x){ return x == output[0]; }) &&
           "Something Went Wrong!!!");


    delete[] a;

    return 0;
}