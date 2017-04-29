#include <bits/stdc++.h>

#include "ZipIter.h"


using namespace std;
using namespace it;




int main ()
{
    vector<int> v { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } ;
    
    set<short> s { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    list<long> l = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    unsigned a[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };



    // for_each(zipIter(v.begin(), s.begin(), l.begin(), begin(a)), 
    //          zipIter(v.end(), s.end(), l.end(), end(a)), [](auto tup){
    //     cout << get<0>(tup) << " " << get<1>(tup) << " " << get<2>(tup) << " " << get<3>(tup) << endl;
    // });


    // for_each(zipIter(v.begin(), s.begin(), l.begin(), begin(a)), 
    //          zipIter(v.end(), s.end(), l.end(), end(a)), unZip([](auto x, auto y, auto w, auto z){
    //     cout << x << " " << y << " " << w << " " << z << endl;
    // }));


    // for_each(ZIP_ALL(v, s, l, a), unZip([](auto x, auto y, auto w, auto z){ 
    //     cout << x << " " << y << " " << w << " " << z << endl;
    // }));


    // for(auto tup : zp)
    //     cout << get<0>(tup) << " " << get<1>(tup) << " " << get<2>(tup) << " " << get<3>(tup) << endl;


    // for(auto& tup : zip(v, s, l, a)) unZip(tup, [](auto x, auto y, auto w, auto z){
    //     cout << x << " " << y << " " << w << " " << z << endl;
    // });


    forEach(v, s, l, a, [](auto x, auto y, auto w, auto z){
        cout << x << " " << y << " " << w << " " << z << endl;
    });



    return 0;
}