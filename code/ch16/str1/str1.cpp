// str1.cpp -- introducting the string class
#include <iostream>
#include <string> // string class
//using string constructors

int main()
{
    using namespace std;
    string piano_man = {'P', 'i', 'a', 'n', 'o', ' ', 'M', 'a', 'n'};
    cout << piano_man << endl;
    char fname[10];
    cin.getline(fname, 10);
    cout << "fname: " << fname << endl;
    return 0;
}