// dma.cpp --dma class methods

#include <cstring>
#include "dma.h"

// baseDMA methods
baseDMA::baseDMA(const char *l, int r)
{
    label = new char[std::strlen(l) + 1];
    std::strcpy(label, l);
    rating = r;
}

baseDMA::baseDMA(const baseDMA &rs)
{
    label = new char[std::strlen(rs.label) + 1];
    std::strcpy(label, rs.label);
    rating = rs.rating;
}

baseDMA::~baseDMA()
{
    delete[] label;
}

baseDMA &baseDMA::operator=(const baseDMA &rs)
{
    if (this == &rs)
        return *this;
    delete[] label;
    label = new char[std::strlen(rs.label) + 1];
    std::strcpy(label, rs.label);
    rating = rs.rating;
    return *this;
}

std::ostream &operator<<(std::ostream &os, const baseDMA &rs)
{
    os << "Label: " << rs.label << std::endl;
    os << "Rating: " << rs.rating << std::endl;
    return os;
}

// lacksDMA methods
lacksDMA::lacksDMA(const char *c, const char *l, int r)
    : baseDMA(l, r)
{
    std::strncpy(color, c, 39);
    color[39] = '\0';
}

lacksDMA::lacksDMA(const char *c, const baseDMA &rs)
    : baseDMA(rs)
{
    std::strncpy(color, c, 39);
    color[39] = '\0';
}

std::ostream &operator<<(std::ostream &os, const lacksDMA &rs)
{
    os << (const baseDMA &)rs;
    os << "Color: " << rs.color << std::endl;
    return os;
}

// hasDMA methods
hasDMA::hasDMA(const char *s, const char *l, int r)
    : baseDMA(l, r)
{
    style = new char[std::strlen(s) + 1];
    std::strcpy(style, s);
}

hasDMA::hasDMA(const char *s, const baseDMA &rs)
    : baseDMA(rs)
{
    style = new char[std::strlen(s) + 1];
    std::strcpy(style, s);
}

hasDMA::hasDMA(const hasDMA &hs)
    : baseDMA(hs) // invoke base class copy constructor
{
    style = new char[std::strlen(hs.style) + 1];
    std::strcpy(style, hs.style);
}

hasDMA::~hasDMA()
{
    delete[] style;
}

hasDMA &hasDMA::operator=(const hasDMA &rs)
{
    if (this == &rs)
        return *this;
    baseDMA::operator=(rs); // copy base portion
    delete[] style;         // prepare for new style
    style = new char[std::strlen(rs.style) + 1];
    std::strcpy(style, rs.style);
    return *this;
}

std::ostream &operator<<(std::ostream &os, const hasDMA &rs)
{
    os << (const baseDMA &)rs;
    os << "Style: " << rs.style << std::endl;
    return os;
}