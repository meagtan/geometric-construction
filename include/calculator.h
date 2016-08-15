#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "geom.h"

// calculates the construction of numbers, points, shapes, etc.
class Calculator : public Constructor {
public:
    Calculator(MoveListener *lis);
    virtual ~Calculator();

    // creates and returns point (n,0) or (0,n)
    const Point *get_point(constr_num n, bool on_y_axis = false);

    // constructs points and lines used in the arithmetic construction of numbers
    const Point *add(constr_num a, constr_num b);
    const Point *sub(constr_num a, constr_num b);
    const Point *mul(constr_num a, constr_num b);
    const Point *div(constr_num a, constr_num b);
    const Point *sqrt(constr_num a);

    const Point *construct_number(constr_num n);
    const Point *construct_point(const Point *p);
};

#endif // CALCULATOR_H
