// studenti.h -- defining a Student class using private inheritance
#ifndef STUDENTI_H_
#define STUDENTI_H_

#include <iostream>
#include <string>
#include <valarray>

class Student : private std::string, private std::valarray<double>
{
private:
    typedef std::valarray<double> ArrayDb;
    std::ostream &arr_out(std::ostream &os) const;
public:
    Student() : std::string("Null Student"), std::valarray<double>() {}
    explicit Student(const std::string &s)
        : std::string(s), std::valarray<double>() {}
    explicit Student(int n) : std::string("Nully"), std::valarray<double>(n) {}
    Student(const std::string &s, int n)
        : std::string(s), std::valarray<double>(n) {}
    Student(const std::string &s, const ArrayDb &a)
        : std::string(s), std::valarray<double>(a) {}
    Student(const char *str, const double *pd, int n)
        : std::string(str), std::valarray<double>(pd, n) {}
    ~Student() {}
    double Average() const;
    const std::string &Name() const;
    double &operator[](int i);
    double operator[](int i) const;
    // friends
    // input
    friend std::istream &operator>>(std::istream &is, Student &stu); // 1 word
    friend std::istream &getline(std::istream &is, Student &stu);    // 1 line
    friend std::ostream &operator<<(std::ostream &os, const Student &stu);
};

#endif