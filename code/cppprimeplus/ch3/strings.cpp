#include <iostream>
#include <string>

int main()
{
    using namespace std;
    string s1;
    getline(cin, s1);
    cout << s1 << endl;
    s1[0] = 'J';
    cout << s1 << endl;
    return 0;
}