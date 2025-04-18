// copyit.cpp -- copy() and iterators
#include <iostream>
#include <iterator>
#include <vector>

int main()
{
    using namespace std;

    int cast[10] = {6, 7, 2, 9, 4, 11, 8, 7, 10, 5};
    vector<int> dice(10);
    //copy from array to vector

    copy(cast, cast + 10, dice.begin());
    cout << "Let the dice be cast!\n";
    //create an ostream iterator
    ostream_iterator<int, char> out_iter(cout, " ");
    //copy from vector to cout
    copy(dice.begin(), dice.end(), out_iter);
    cout << endl;
    cout<<"Implicit use of reverse iterator.\n";
    copy(dice.rbegin(), dice.rend(), out_iter);
    cout << endl;
    cout << "Explicit use of reverse iterator.\n";
    vector<int>::reverse_iterator ri;
    for (ri = dice.rbegin(); ri != dice.rend(); ++ri)
        cout << *ri << " ";
    cout << endl;

    return 0;
}