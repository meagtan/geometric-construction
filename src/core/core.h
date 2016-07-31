#ifndef CORE_H
#define CORE_H

#include <vector>
#include <string>
#include <algorithm>
#include "../num/constr_num.h"

using std::vector;
using std::pair;
using std::string;

// consider all integers already constructed
constexpr bool auto_construct_ints = true;

struct Point {
    const constr_num x, y; // TODO implement constructible numbers following BNF syntax, with casting

    Point(constr_num x = 0, constr_num y = 0);

    constr_num distance(const Point &other) const;

    // TODO overloaded operators for comparison, copy constructor etc.
};

struct Line {
    const constr_num x_coeff, y_coeff, const_coeff; // x_coeff * x + y_coeff * y + const_coeff == 0

    Line(const Point &a, const Point &b);
    Line(constr_num x_coeff, constr_num y_coeff, constr_num const_coeff);

    constr_num norm() const; // x_coeff ^ 2 + y_coeff ^ 2
    constr_num value_at(const Point &a) const; // substitute a for the equation of the line
    constr_num distance(const Point &a) const;

    bool contains(const Point &a) const;
};

struct Circle {
    const Point center;
    const constr_num radius;

    Circle(const Point &center, const Point &other);
    Circle(const Point &center, constr_num radius);

    constr_num value_at(const Point &a) const;
    constr_num distance(const Point &a) const;

    bool contains(const Point &a) const;
};

struct _Move {};

template<typename T1, typename T2, typename R>
struct Move : _Move {
    enum move { compass = 'c', straightedge = 's', meet = 'm' };
    T1 arg1;
    T2 arg2;
    R result;
};

class Scope {
private:
    vector<Point*>  points;
    vector<Line*>   lines;
    vector<Circle*> circles;
    vector<_Move>   moves;

    // TODO store names for each point, line and circle

    void add(Point *a);
    void add(Line *l);
    void add(Circle *c);

public:
    Scope();
    ~Scope();

    bool contains(Point *a) const;
    bool contains(Line *l) const;
    bool contains(Circle *c) const;

    Line *join_line(Point &a, Point &b, bool check_contains = true);
    Circle *join_circle(Point &c, Point &o, bool check_contains = true);

    Point *meet(Line &a, Line &b, bool check_contains = true);
    pair<Point*,Point*> meet(Line &a, Circle &b, bool check_contains = true);
    pair<Point*,Point*> meet(Circle &a, Circle &b, bool check_contains = true);
};

const Point origin = Point();

#endif // CORE_H
