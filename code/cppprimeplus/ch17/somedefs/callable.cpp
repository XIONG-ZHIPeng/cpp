// callable.cpp -- callable types and templates
#include "somedefs.h"
#include <iostream>
#include <functional>

double dub(double x) {return 2.0 * x;}
double square(double x) {return x * x;}

int main()
{
    using std::cout;
    using std::endl;
    using std::function;


    double y = 1.21;
    function<double(double)> f1 = dub;
    function<double(double)> f2 = square;
    function<double(double)> f3 = Fq(10.0);
    function<double(double)> f4 = Fp(10.0);
    function<double(double)> f5 = [](double x) {return x * x;};
    function<double(double)> f6 = [](double x) {return x+x/2.0;};
    cout << "Function pointer dub:\n";
    cout << "   " << use_f(y, f1) << endl;
    cout << "Function pointer square:\n";
    cout << "   " << use_f(y, f2) << endl;
    cout << "Function object Fp:\n";
    cout << "   " << use_f(y, f3) << endl;
    cout << "Function object Fq:\n";
    cout << "   " << use_f(y, f4) << endl;
    cout << "Lambda expression 1:\n";
    cout << "   " << use_f(y, f5) << endl;
    cout << "Lambda expression 2:\n";
    cout << "   " << use_f(y, f6) << endl;
    return 0;
}