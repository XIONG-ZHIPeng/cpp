//error1.cpp -- using the abort() function
#include <iostream>
#include <cstdlib> // for abort()
double hmean(double a, double b);

int main()
{
    using namespace std;
    double x, y, z;
    cout << "Enter two numbers: ";
    while (cin >> x >> y)
    {
        z = hmean(x, y);
        cout << "Harmonic mean of " << x << " and " << y
             << " is " << z << endl;
        cout << "Enter next pair (q to quit): ";
    }
    cout << "Bye.\n";
    return 0;
}
double hmean(double a, double b)
{
    using namespace std;
    if (a == -b)
    {
        cout << "hmean(" << a << ", " << b << "): "
             << "Invalid arguments: a = -b\n";
        abort(); // terminate the program
    }
    return 2.0 * a * b / (a + b);
}