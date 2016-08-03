#ifndef CORE_H
#define CORE_H

#include <vector>
#include <string>
#include <algorithm>
#include "constr_num.h"

using std::vector;
using std::pair;
using std::string;

// consider all integers already constructed
constexpr bool auto_construct_ints = true;

struct Point {
    const constr_num x, y; // TODO implement constructible numbers following BNF syntax, with casting

    Point(constr_num x = 0, constr_num y = 0);
    Point(const Point &other) = default;
    virtual ~Point() = default;

    constr_num distance(const Point &other) const;

    // TODO overloaded operators for comparison, copy constructor etc.
    bool operator==(const Point &other) const;
    bool operator!=(const Point &other) const;
};

struct Circle; // forward declare for Line::meet(const Circle &)

struct Line {
    const constr_num x_coeff, y_coeff, const_coeff; // x_coeff * x + y_coeff * y + const_coeff == 0

    Line(const Point &a, const Point &b);
    Line(constr_num x_coeff, constr_num y_coeff, constr_num const_coeff);
    Line(const Line &other) = default;
    virtual ~Line() = default;

    constr_num norm() const; // x_coeff ^ 2 + y_coeff ^ 2
    constr_num value_at(const Point &a) const; // substitute a for the equation of the line
    constr_num distance(const Point &a) const;

    bool contains(const Point &a) const;

    // if point a precedes point b in line based on the direction of the vector (x_coeff, y_coeff)
    bool precedes(const Point &a, const Point &b) const;

    bool operator==(const Line &other) const;
    bool operator!=(const Line &other) const;

    const Point *meet(const Line &other) const;
    pair<const Point*,const Point*> meet(const Circle &other) const;

protected:
    // set boundaries to the points along the line that the object contains, to be overridden for line segments, etc.
    virtual bool within_boundary(const Point &a) const;
};

struct Circle {
    const Point center;
    const constr_num radius;

    Circle(const Point &center, const Point &other);
    Circle(const Point &center, constr_num radius);
    Circle(const Circle &other) = default;
    virtual ~Circle() = default;

    constr_num value_at(const Point &a) const;
    constr_num distance(const Point &a) const;

    bool contains(const Point &a) const;

    bool operator==(const Circle &other) const;
    bool operator!=(const Circle &other) const;

    pair<const Point*,const Point*> meet(const Line &other) const;
    pair<const Point*,const Point*> meet(const Circle &other) const;

protected:
    // set boundaries to the points along the line that the object contains, to be overridden for arcs, etc.
    virtual bool within_boundary(const Point &a) const;
};

struct _Move {};

enum struct MoveType { compass = 'c', straightedge = 's', meet = 'm' };

template<typename T1, typename T2, typename R>
struct Move : _Move {
    const MoveType move;
    const T1 *arg1;
    const T2 *arg2;
    const R *result;

    Move(const MoveType move, const T1 *arg1, const T2 *arg2, const R *result);
    Move(const Move &other) = default;
    ~Move() = default;
};

typedef void (*MoveListener)(_Move move);

class Scope {
protected:
    vector<const Point*>  points;
    vector<const Line*>   lines;
    vector<const Circle*> circles;
    MoveListener listener;

    // TODO store names for each point, line and circle

    void add(const Point *a);
    void add(const Line *l);
    void add(const Circle *c);

public:
    Scope(MoveListener listener);
    ~Scope();

    bool contains(const Point *a) const;
    bool contains(const Line *l) const;
    bool contains(const Circle *c) const;

    const Line *join_line(const Point &a, const Point &b);
    const Circle *join_circle(const Point &c, const Point &o);

    const Point *meet(const Line &a, const Line &b);
    pair<const Point*,const Point*> meet(const Line &a, const Circle &b);
    pair<const Point*,const Point*> meet(const Circle &a, const Circle &b);
};

const Point * const origin = new Point(),
            * const unit_x = new Point(1);
const Line  * const x_axis = new Line(0, 1, 0),
            * const y_axis = new Line(1, 0, 0);

#endif // CORE_H