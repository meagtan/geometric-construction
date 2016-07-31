#include "core.h"

Scope::Scope() {}

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
Line *Scope::join_line(Point &a, Point &b, bool check_contains)
{
    if (check_contains && (!this->contains(&a) || !this->contains(&b)))
        return nullptr;

    Line *l = new Line(a, b);
    this->add(l); // TODO prefix AB
    if (check_contains)
        this->moves.push_back(Move<Point,Point,Line>(MoveType::straightedge, &a, &b, l));
    return l;
}

// draw circle centered in a and touching b
Circle *Scope::join_circle(Point &a, Point &b, bool check_contains)
{
    if (check_contains && (!this->contains(&a) || !this->contains(&b)))
        return nullptr;

    Circle *c = new Circle(a, b);
    this->add(c); // TODO prefix A
    if (check_contains)
        this->moves.push_back(Move<Point,Point,Circle>(MoveType::compass, &a, &b, c));
    return c;
}

// TODO make meet check for containment, call a's own _meet method, and name and add the resulting points it gives

// intersect two lines
Point *Scope::meet(Line &a, Line &b, bool check_contains)
{
    if (check_contains && (!this->contains(&a) || !this->contains(&b)))
        return nullptr;

    constr_num det = a.x_coeff * b.y_coeff - a.y_coeff * b.x_coeff;

    // parallel lines
    if (det == 0)
        return nullptr;

    // might be neater with matrices
    constr_num x = (a.const_coeff * b.y_coeff - a.y_coeff * b.const_coeff) / det,
               y = (b.const_coeff * a.x_coeff - b.x_coeff * a.const_coeff) / det;

    Point* p = new Point(x, y);
    this->add(p); // TODO prefix
    if (check_contains)
        this->moves.push_back(Move<Line,Line,Point>(MoveType::meet, &a, &b, p));
    return p;
}

// intersect a line and a circle
pair<Point*,Point*> Scope::meet(Line &a, Circle &b, bool check_contains)
{
    if (check_contains && (!this->contains(&a) || !this->contains(&b)))
        return pair<Point*,Point*>(nullptr, nullptr);

    constr_num value = a.value_at(b.center),
               discr = b.radius * b.radius * a.norm() - value * value,
               x_offset = b.center.x + a.x_coeff * value / a.norm(),
               y_offset = b.center.y + a.y_coeff * value / a.norm();
    Point *p1 = nullptr, *p2 = nullptr;

    if (discr == 0) {
        p1 = new Point(x_offset, y_offset);
        this->add(p1);
        if (check_contains)
            this->moves.push_back(Move<Line,Circle,Point>(MoveType::meet, &a, &b, p1));
    } else if (discr > 0) {
        constr_num x_factor = a.y_coeff * sqrt(discr) / a.norm(),
                   y_factor = a.x_coeff * sqrt(discr) / a.norm();

        p1 = new Point(x_offset + x_factor,
                       y_offset - y_factor);
        p2 = new Point(x_offset - x_factor,
                       y_offset + y_factor);

        this->add(p1); // TODO prefix
        this->add(p2); // TODO prefix
        if (check_contains)
            this->moves.push_back(Move<Line,Circle,pair<Point*,Point*>>(MoveType::meet, &a, &b, new pair<Point*,Point*>(p1, p2)));
    }

    return pair<Point*,Point*>(p1, p2);
}

// intersect two circles
pair<Point*,Point*> Scope::meet(Circle &a, Circle &b, bool check_contains)
{
    if (check_contains && (!this->contains(&a) || !this->contains(&b)))
        return pair<Point*,Point*>(nullptr, nullptr);

    Line l ((b.center.x - a.center.x) * 2,
            (b.center.y - a.center.y) * 2,
            a.value_at(origin) - b.value_at(origin));

    pair<Point*,Point*> res = this->meet(l, a, false);

    if (check_contains) {
        if (res.second != nullptr)
            this->moves.push_back(Move<Circle,Circle,pair<Point*,Point*>>(MoveType::meet, &a, &b, new pair<Point*,Point*>(res.first, res.second)));
        else if (res.first != nullptr)
            this->moves.push_back(Move<Circle,Circle,Point>(MoveType::meet, &a, &b, res.first));
    }

    return res;
}
