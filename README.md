# ZipIterator
C++14 simple implementation of a iterator zipper


This is a single header file that implements iterator zipping. 
It's main purpose is to be simple and to work in conjunction with stl algorithms.
No dependences or installation. Just include and use.

Example:


```C++
int main ()
{
  std::vector<int> v = {1, 2, 3, 4, 5};
  std::array<double, 5> u = {5, 4, 3, 2, 1};
  

  // Returns a tuple with the iterators passed to zip::zipIterator

  std::for_each(zip::zipIterator(v.begin(), u.begin()), zip::zipIterator(v.end(), u.end()), [](auto&& tup) {
    
    std::cout << *std::get<0>(tup) << "   " << *std::get<1>(tup) << std::endl;

  });


  // Unpacks the tuple and passes to a lambda

  std::for_each(zip::zipIterator(v.begin(), u.begin()), zip::zipIterator(v.end(), u.end()), zip::unZip([](auto&& x, auto&& y) {
    
    std::cout << *x << "   " << *y << std::endl;

  }));


  return 0;
}
```
