#include <src/core/core.h>

Scope::Scope() : points(), lines(), circles()
{
    // TODO
}

void Scope::add(Point *const a, string prefix)
{

}

void Scope::add(Line *const l, string prefix)
{

}

void Scope::add(Circle *const c, string prefix)
{

}

bool Scope::contains(Point *const a) const
{
    return (auto_construct_ints && a->x.is_int() && a->y.is_int()) ||
           this->points.count(a);
}

bool Scope::contains(Line *const l) const
{
    return this->lines.count(l);
}

bool Scope::contains(Circle *const c) const
{
    return this->circles.count(c);
}

// draw line connecting two points
Line *Scope::join_line(Point &a, Point &b, bool check_contains)
{
    if (check_contains && (!this->contains(&a) || !this->contains(&b)))
        return nullptr;

    Line *l = new Line(a, b);
    this->add(l); // TODO prefix AB
    return l;
}

// draw circle centered in a and touching b
Circle *Scope::join_circle(Point &a, Point &b, bool check_contains)
{
    if (check_contains && (!this->contains(&a) || !this->contains(&b)))
        return nullptr;

    Circle *c = new Circle(a, b);
    this->add(c); // TODO prefix A
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
    } else if (discr > 0) {
        constr_num x_factor = a.y_coeff * sqrt(discr) / a.norm(),
                   y_factor = a.x_coeff * sqrt(discr) / a.norm();

        p1 = new Point(x_offset + x_factor,
                       y_offset - y_factor);
        p2 = new Point(x_offset - x_factor,
                       y_offset + y_factor);

        this->add(p1); // TODO prefix
        this->add(p2); // TODO prefix
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

    return this->meet(l, a, false);
}
