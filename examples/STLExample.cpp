/** 
  *  \file STLExample.cpp
  *  
  *	These are some examples showing how to use the 'zip' and 'zipIter'
  * utilities on stl functions. Once again, to modify the items 
  * returned by acessing the iterators, use a copy. The value returned
  * is a rvalue tuple of references, you cannot take a lvalue
  * reference to it.
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





/// To print a tuple
template <typename... Args, size_t... Is>
void printTup(ostream& out, const tuple<Args...>& tup, index_sequence<Is...>)
{
	auto dum = {((out << (Is == 0 ? "" : "  ") << get<Is>(tup)), 0)...};
}


template <typename... Args>
ostream& operator << (ostream& out, const tuple<Args...>& tup)
{
	printTup(out, tup, make_index_sequence<sizeof...(Args)>());

	return out;
}


/// Printing every element in a Zip class
template <typename... Containers>
ostream& operator << (ostream& out, const Zip<Containers...>& zip)
{
	for(auto tup : zip)
		out << tup << '\n';

	return out;
}






int main ()
{
	/// Initializing some variables
	int n = 10, j = 0;

	vector<int> v(n), u(n);


	// generate(v.begin(), v.end(), []{ return rngInt(0, 10); });
	// iota(u.begin(), u.end(), 0);

	/// The return is a tuple here. Its the same as above.
	generate(ZIP_ALL(v, u), [&j]{ return make_tuple(rngInt(0, 10), j++); });



	cout << "Before sort:\n\n" << zip(v, u) << "\n\n";

	/** Sorting a container along with its indexes, just like Matlab sorting.
	  * As the 'std::tuple' has a 'operator<', we can simply do this:
	*/
	sort(ZIP_ALL(v, u));


	cout << "After: \n\n" << zip(v, u) << "\n\n\n";


	/** Using a summation with unZip. Here, all arguments are
	  * expanded and passed to unZip. The first is the accumulated sum,
	  * and the second and third are the elements of the tuple.
	 */
	double res = accumulate(ZIP_ALL(v, u), 0, unZip([](int sum, int x, int y){ return sum + x + y; }));

	cout << "Result of the sum:    " << res << "\n\n\n\n";




	/// Another example using the unZip. 
	transform(ZIP_ALL(v, u), zipBegin(v, u), unZip([](int x, int y){ return make_tuple(x, y); }));

	cout << "Transformed: \n\n" << zip(v, u) << "\n\n\n";



	/** Another one using now the zipIter. As the first elements defines 
	  * the range, all the others must have equal or greater length
	*/
	reverse(zipIter(v.begin() + 2, u.begin()), zipIter(v.begin() + 5, u.end()));

	/// This will crash
	// reverse(zipIter(v.begin(), u.begin() + 3), zipIter(v.begin(), u.end()));

	cout << "Reversing some items: \n\n" << zip(v, u) << "\n\n\n";





    return 0;
}