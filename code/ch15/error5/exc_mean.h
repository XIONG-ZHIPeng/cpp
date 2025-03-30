// exc_mean.h -- exception classes for hmean(), gmean()
#ifndef EXC_MEAN_H_
#define EXC_MEAN_H_
#include <iostream>

class bad_hmean {
private:
    double v1;
    double v2;
public:
    bad_hmean(double a, double b) : v1(a), v2(b) {}
    void mesg() const {
        std::cout << "bad_hmean(" << v1 << ", " << v2 << "): "
                  << "invalid arguments: a = -b\n";
    }
};

class bad_gmean {
public:
    double v1;
    double v2;
    bad_gmean(double a, double b) : v1(a), v2(b) {}
    const char* mesg() const {
        return "bad_gmean(): arguments should be >= 0";
    }
};

#endif