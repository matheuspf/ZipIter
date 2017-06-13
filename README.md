# ZipIter
This project is a header only implementation of an iterator zipper made in C++14.

You can iterate and use stl algorithms on multiple iterators at the same time easily with no runtime overhead (using -O3 optimization flag).

No dependences or installation, just include and use.

The library was tested on both g++ 6.2.0 and clang 3.9.1, and requires the ``-std=c++14`` flag.
<br>

### Google Test


There are a number of tests using [Google Test](https://github.com/google/googletest).

If you want to run the tests:

```
cd test
mkdir build
cd build

cmake ..
cmake --build .

./ZipIterTests
```

Google Test will be downloaded automatically from the repository.
<br>

###Documentation

If you want to generate the documentation, install [Doxygen](http://www.stack.nl/~dimitri/doxygen/) and run:

```
cd doc
doxygen Doxyfile
```

<br>

### Examples

If you want to build and run some examples:

```
cd examples
mkdir build
cd build

cmake ..
cmake --build .

./LoopingExample
./STLExample
```


<br>
Here are some:

```c++

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

#include "ZipIter/ZipIter.h"  // The header file

using namespace std;
using namespace it;    // ZipIter namespace


int main ()
{
	vector<int> v = {1, 2, 3, 4, 5};
	array<double, 5> u = {5, 4, 3, 2, 1};



	// Iterating through both containers using std::for_each
	for_each(zipBegin(v, u), zipEnd(v, u), [](auto tup){
		cout << get<0>(tup) << "     " << get<1>(tup) << "\n";
	});



	// Using the unZip to unpack those values.
	// They can be taken as references too
	for_each(zipBegin(v, u), zipEnd(v, u), unZip([](int x, double y){
		cout << x << "     " << y << "\n";
	}));



	// Using for range -- The return is a tuple containing references
	for(auto tup : zip(v, u)) unZip(tup, [](int x, double y){
		cout << x << "     " << y << "\n";
	});



	// Or using a function that encapsulates the above.
	// The lambda comes after the variadic arguments 
	forEach(v, u, [](int x, double y){
		cout << x << "     " << y << "\n";
	});




	// Sorting both containers using the std::tuple operator <
	sort(zipIter(v.begin(), u.begin()), zipIter(v.end(), u.end()));


	// or
	sort(zipBegin(v, u), zipEnd(v, u));


	// or even using a macro that does exactly the same as above
	sort(ZIP_ALL(v, u));


	// using a custom comparison
	sort(ZIP_ALL(v, u), [](auto tup1, auto tup2){
		return get<0>(tup1) + get<1>(tup1) < get<0>(tup2) + get<1>(tup2);
	});


	// or using the unZip to magically unpack those tuples
	sort(ZIP_ALL(v, u), unZip([](int v1, double u1, int v2, double u2){
		return v1 + u1 < v2 + u2;
	}));



	// It is really that easy
	transform(ZIP_ALL(v, u), zipBegin(v, u), unZip([](int x, double y){
		return make_tuple(0, 0.0);
	}));


	reverse(ZIP_ALL(v, u));


	accumulate(ZIP_ALL(v, u), 0.0, unZip([](double sum, int x, double y){
		return sum + x + y;
	}));




  return 0;
}
```
<br>
