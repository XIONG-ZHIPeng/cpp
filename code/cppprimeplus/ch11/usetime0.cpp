// usetime0.cpp -- using the first draft of the Time class
// complie usetime0.cpp and mytime0.cpp together
#include <iostream>
#include "mytime0.h"

int main1()
{
    using std::cout;
    using std::endl;
    // Time planning;
    // Time coding(2, 40);
    // Time fixing(5, 55);
    // Time total;

    // cout << "planning time = ";
    // planning.Show();
    // cout << endl;

    // cout << "coding time = ";
    // coding.Show();
    // cout << endl;

    // cout << "fixing time = ";
    // fixing.Show();
    // cout << endl;

    // total = coding + fixing;
    // cout << "coding.operator+(fixing) = ";
    // total.Show();
    // cout << endl;

    // Time weeding(4, 35);
    // Time waxing(2, 47);
    // Time total;
    // Time diff;
    // Time adjusted;

    // cout << "weeding time = ";
    // weeding.Show();
    // cout << endl;

    // cout << "waxing time = ";
    // waxing.Show();
    // cout << endl;

    // cout << "total work time = ";
    // total = weeding + waxing;
    // total.Show();
    // cout << endl;

    // cout << "weeding.operator-(waxing) = ";
    // diff = weeding - waxing;
    // diff.Show();
    // cout << endl;

    // cout << "adjusted work time = ";
    // adjusted = total * 1.5;
    // adjusted.Show();
    // cout << endl;

    // Time A = 2.75 * total;
    // cout << "2.75 * total = ";
    // cout << A << " Test!!! " << endl;

    Time aida(3, 35);
    Time tosca(2, 48);
    Time temp;

    cout << "Aida and Tosca:\n";
    cout << aida << "; " << tosca << endl;
    temp = aida + tosca;
    cout << "Aida + Tosca = " << temp << endl;
    temp = aida * 1.17;
    cout << "Aida * 1.17 = " << temp << endl;
    cout << "10 * Tosca = " << 10 * tosca << endl;

    return 0;
}