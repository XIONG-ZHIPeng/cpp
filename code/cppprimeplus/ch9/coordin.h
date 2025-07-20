#ifndef COORDIN_H_
#define CORRDIN_H_

struct polar
{
    /* data */
    double distance;
    double angle;
};

struct rect
{
    double x;
    double y;
};

//prototypes
polar rect_to_polar(rect xpos);
void show_polar(polar dapos);



#endif
