//error2.cpp -- returning an error code
#include <iostream>
#include <cfloat>

bool hmean(double a, double b, double * ans);

int main()
{
    using namespace std;
    double x, y, z;

    cout << "Enter two numbers: ";
    while (cin >> x >> y)
    {
        if (hmean(x, y, &z))
            cout << "Harmonic mean of " << x << " and " << y
                 << " is " << z << endl;
        else
            cout << "Error: Invalid arguments to hmean()\n";
        cout << "Enter next pair (q to quit): ";
    }
    cout << "Bye.\n";
    return 0;
}

bool hmean(double a, double b, double * ans)
{
    if (a == -b)
    {
        *ans = DBL_MAX; // set ans to max double value
        return false;
    }
    *ans = 2.0 * a * b / (a + b);
    return true;
}