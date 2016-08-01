#include "include/core.h"

Scope::Scope(MoveListener listener) : listener(listener) {}

Scope::~Scope()
{
    for (auto &p : points)
        delete p;
    for (auto &p : lines)
        delete p;
    for (auto &p : circles)
        delete p;
}

// TODO do not add already existing points, lines and circles

void Scope::add(Point *a)
{
    this->points.push_back(a);
}

void Scope::add(Line *l)
{
    this->lines.push_back(l);
}

void Scope::add(Circle *c)
{
    this->circles.push_back(c);
}

bool Scope::contains(Point *a) const
{
    return (auto_construct_ints && a->x.is_int() && a->y.is_int()) ||
           std::count(this->points.begin(), this->points.end(), a);
}

bool Scope::contains(Line *l) const
{
    return std::count(this->lines.begin(), this->lines.end(), l);
}

bool Scope::contains(Circle *c) const
{
    return std::count(this->circles.begin(), this->circles.end(), c);
}

#define _make_move(movetype, arg1, arg1type, arg2, arg2type, res, restype) do {               \
    if (res != nullptr) {                                                                     \
        this->add(res);                                                                       \
        this->listener(Move<arg1type,arg2type,restype>(MoveType::movetype, arg1, arg2, res)); \
}} while (0)

// draw line connecting two points
Line *Scope::join_line(Point &a, Point &b)
{
    if (!this->contains(&a) || !this->contains(&b))
        return nullptr;

    Line *l = new Line(a, b);
    _make_move(straightedge, &a, Point, &b, Point, l, Line);

    return l;
}

// draw circle centered in a and touching b
Circle *Scope::join_circle(Point &a, Point &b)
{
    if (!this->contains(&a) || !this->contains(&b))
        return nullptr;

    Circle *c = new Circle(a, b);
    _make_move(compass, &a, Point, &b, Point, c, Circle);

    return c;
}

// intersect two lines
Point *Scope::meet(Line &a, Line &b)
{
    if (!this->contains(&a) || !this->contains(&b))
        return nullptr;

    Point *p = a.meet(b);
    _make_move(meet, &a, Line, &b, Line, p, Point);

    return p;
}

// intersect a line and a circle
pair<Point*,Point*> Scope::meet(Line &a, Circle &b)
{
    if (!this->contains(&a) || !this->contains(&b))
        return pair<Point*,Point*>(nullptr, nullptr);

    auto res = b.meet(a);

    _make_move(meet, &a, Line, &b, Circle, res.first, Point);
    _make_move(meet, &a, Line, &b, Circle, res.second, Point);

    return res;
}

// intersect two circles
pair<Point*,Point*> Scope::meet(Circle &a, Circle &b)
{
    if (!this->contains(&a) || !this->contains(&b))
        return pair<Point*,Point*>(nullptr, nullptr);

    auto res = a.meet(b);

    _make_move(meet, &a, Circle, &b, Circle, res.first, Point);
    _make_move(meet, &a, Circle, &b, Circle, res.second, Point);

    return res;
}

#undef _make_move
