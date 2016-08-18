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
    const Point *get_add(constr_num a, constr_num b, bool on_y_axis = false);
    const Point *get_sub(constr_num a, constr_num b, bool on_y_axis = false);
    const Point *get_mul(constr_num a, constr_num b, bool on_y_axis = false);
    const Point *get_div(constr_num a, constr_num b, bool on_y_axis = false);
    const Point *get_sqrt(constr_num a, bool on_y_axis = false);

    const Point *construct_number(constr_num n, bool on_y_axis = false);
    const Point *construct_point(const Point &p);
};

#endif // CALCULATOR_H
