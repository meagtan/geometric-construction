#include "include/core.h"

// Moves

template<typename T1, typename T2, typename R>
Move<T1,T2,R>::Move(const MoveType move, const T1 *arg1, const T2 *arg2, const R *result) :
    move(move), arg1(arg1), arg2(arg2), result(result) {}

// Scope

Scope::Scope(MoveListener listener) : listener(listener)
{
    add(origin);
    add(unit_x);
    add(x_axis);
    add(y_axis);
}

Scope::~Scope()
{
    for (auto &p : points)
        delete p;
    for (auto &p : lines)
        delete p;
    for (auto &p : circles)
        delete p;
}

// TODO Scope::add should not check Scope::contains, but only if points contains the given pointer
//   e.g. for points considered to be contained but not really in points
// Then Scope::add should be called by join_line and join_circle

void Scope::add(const Point *a)
{
    if (!contains(a))
        points.push_back(a);
}

void Scope::add(const Line *l)
{
    if (!contains(l))
        lines.push_back(l);
}

void Scope::add(const Circle *c)
{
    if (!contains(c))
        circles.push_back(c);
}

bool Scope::contains(const Point *a) const
{
    return a != nullptr &&
           ((auto_construct_ints && a->x.is_int() && a->y.is_int()) ||
            std::count_if(points.begin(), points.end(), [a](const Point *p) {return *p == *a; }));
}

bool Scope::contains(const Line *l) const
{
    return l != nullptr &&
           std::count_if(lines.begin(), lines.end(), [l](const Line *p) {return *p == *l; });
}

bool Scope::contains(const Circle *c) const
{
    return c != nullptr &&
           std::count_if(circles.begin(), circles.end(), [c](const Circle *p) {return *p == *c; });
}

#define _make_move(movetype, arg1, arg1type, arg2, arg2type, res, restype) do {               \
    if (res != nullptr) {                                                                     \
        add(res);                                                                       \
        listener(Move<arg1type,arg2type,restype>(MoveType::movetype, arg1, arg2, res)); \
}} while (0)

// draw line connecting two points
const Line *Scope::join_line(const Point &a, const Point &b)
{
    if (!contains(&a) || !contains(&b))
        return nullptr;

    const Line *l = new Line(a, b);
    _make_move(straightedge, &a, Point, &b, Point, l, Line);

    return l;
}

// draw circle centered in a and touching b
const Circle *Scope::join_circle(const Point &a, const Point &b)
{
    if (!contains(&a) || !contains(&b))
        return nullptr;

    const Circle *c = new Circle(a, b);
    _make_move(compass, &a, Point, &b, Point, c, Circle);

    return c;
}

// intersect two lines
const Point *Scope::meet(const Line &a, const Line &b)
{
    if (!contains(&a) || !contains(&b))
        return nullptr;

    const Point *p = a.meet(b);
    _make_move(meet, &a, Line, &b, Line, p, Point);

    return p;
}

// intersect a line and a circle
pair<const Point*,const Point*> Scope::meet(const Line &a, const Circle &b)
{
    if (!contains(&a) || !contains(&b))
        return pair<const Point*,const Point*>(nullptr, nullptr);

    auto res = b.meet(a);

    _make_move(meet, &a, Line, &b, Circle, res.first, Point);
    _make_move(meet, &a, Line, &b, Circle, res.second, Point);

    return res;
}

// intersect two circles
pair<const Point*,const Point*> Scope::meet(const Circle &a, const Circle &b)
{
    if (!contains(&a) || !contains(&b))
        return pair<Point*,Point*>(nullptr, nullptr);

    auto res = a.meet(b);

    _make_move(meet, &a, Circle, &b, Circle, res.first, Point);
    _make_move(meet, &a, Circle, &b, Circle, res.second, Point);

    return res;
}

#undef _make_move
