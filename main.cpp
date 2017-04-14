#include <bits/stdc++.h>

#include "ZipIterator.h"


using namespace std;



int main ()
{
    vector<int> v { 0, 1, 2, 3, 4 } ;

    array<double, 5> u { 1, 2, 3, 4, 5 };


    // for_each(it::zipIterator(v.begin(), u.begin()), it::zipIterator(v.end(), u.end()), [](auto&& tup){
    //     cout << get<0>(tup) << "      " << get<1>(tup) << "\n";
    // });

    // for_each(it::zipIterator(v.begin(), u.begin()), it::zipIterator(v.end(), u.end()), it::unZip([](int x, double y){
    //     cout << x << "      " << y << "\n";
    // }));

    // for(auto&& tup : it::zip(v, u))
        // cout << get<0>(tup) << "      " << get<1>(tup) << "\n";

    // for(auto&& tup : it::zip(v, u)) it::unZip(tup, [](int x, double y){
    //     cout << x << "      " << y << "\n";
    // });


    it::forEach(it::zip(v, u), [](int x, double y){ cout << x << "      " << y << "\n"; });





    return 0;
}