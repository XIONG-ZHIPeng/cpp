// error3.cpp -- using an exception
#include <iostream>
double hmean(double a, double b); // function prototype

int main()
{
    double x, y, z;
    std::cout << "Enter two numbers: ";
    while (std::cin >> x >> y)
    {
        try
        {
            z = hmean(x, y);
            std::cout << "Harmonic mean of " << x << " and " << y
                      << " is " << z << std::endl;
        }
        catch (const char * s)
        {
            std::cout << s << std::endl;
            std::cout << "Enter a new pair of numbers: ";
            continue;
        }
        std::cout << "Enter next pair: ";
    }
    std::cout << "Bye.\n";
    return 0;
} // main() ends here

double hmean(double a, double b) // function definition
{
    if (a == -b)
        throw "bad hmean() arguments: a = -b";
    return 2.0 * a * b / (a + b);
} // hmean() ends here