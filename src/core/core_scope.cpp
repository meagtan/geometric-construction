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

// draw line connecting two points
Line *Scope::join_line(Point &a, Point &b)
{
    if (!this->contains(&a) || !this->contains(&b))
        return nullptr;

    Line *l = new Line(a, b);
    this->add(l); // TODO prefix AB
    this->listener(Move<Point,Point,Line>(MoveType::straightedge, &a, &b, l));

    return l;
}

// draw circle centered in a and touching b
Circle *Scope::join_circle(Point &a, Point &b)
{
    if (!this->contains(&a) || !this->contains(&b))
        return nullptr;

    Circle *c = new Circle(a, b);
    this->add(c); // TODO prefix A
    this->listener(Move<Point,Point,Circle>(MoveType::compass, &a, &b, c));

    return c;
}

// intersect two lines
Point *Scope::meet(Line &a, Line &b)
{
    if (!this->contains(&a) || !this->contains(&b))
        return nullptr;

    Point *p = a.meet(b);
    this->add(p);
    this->listener(Move<Line,Line,Point>(MoveType::meet, &a, &b, p));

    return p;
}

// intersect a line and a circle
pair<Point*,Point*> Scope::meet(Line &a, Circle &b)
{
    if (!this->contains(&a) || !this->contains(&b))
        return pair<Point*,Point*>(nullptr, nullptr);

    auto res = b.meet(a);

    if (res.first != nullptr) {
        this->add(res.first);
        this->listener(Move<Line,Circle,Point>(MoveType::meet, &a, &b, res.first));
    }
    if (res.second != nullptr) {
        this->add(res.second);
        this->listener(Move<Line,Circle,Point>(MoveType::meet, &a, &b, res.second));
    }

    return res;
}

// intersect two circles
pair<Point*,Point*> Scope::meet(Circle &a, Circle &b)
{
    if (!this->contains(&a) || !this->contains(&b))
        return pair<Point*,Point*>(nullptr, nullptr);

    auto res = a.meet(b);

    if (res.first != nullptr) {
        this->add(res.first);
        this->listener(Move<Circle,Circle,Point>(MoveType::meet, &a, &b, res.first));
    }
    if (res.second != nullptr) {
        this->add(res.second);
        this->listener(Move<Circle,Circle,Point>(MoveType::meet, &a, &b, res.second));
    }

    return res;
}
