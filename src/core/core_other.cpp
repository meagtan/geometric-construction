#include "include/core.h"

// Point

Point::Point(constr_num x, constr_num y) : x(x), y(y) {}

constr_num Point::distance(const Point &other) const
{
    return sqrt(x * other.x + y * other.y);
}

bool Point::operator==(const Point &other) const
{
    return x == other.x && y == other.y;
}

bool Point::operator!=(const Point &other) const
{
    return !operator ==(other);
}

// Line

Line::Line(constr_num x_coeff, constr_num y_coeff, constr_num const_coeff) :
    x_coeff(x_coeff), y_coeff(y_coeff), const_coeff(const_coeff) {}

Line::Line(const Point &a, const Point &b) :
    x_coeff(a.y - b.y),
    y_coeff(b.x - a.x),
    const_coeff(a.x * b.y - a.y * b.x) {}

constr_num Line::norm() const
{
    return x_coeff * x_coeff + y_coeff * y_coeff;
}

constr_num Line::value_at(const Point &a) const
{
    return x_coeff * a.x + y_coeff * a.y + const_coeff;
}

constr_num Line::distance(const Point &a) const
{
    return abs(value_at(a)) / sqrt(norm());
}

bool Line::contains(const Point &a) const
{
    return value_at(a) == 0 && within_boundary(a);
}

bool Line::precedes(const Point &a, const Point &b) const
{
    return contains(a) && contains(b) &&
           x_coeff * (b.x - a.x) > 0;
}

bool Line::operator==(const Line &other) const
{
    return x_coeff * other.const_coeff == const_coeff * other.x_coeff &&
           y_coeff * other.const_coeff == const_coeff * other.y_coeff;
}

bool Line::operator!=(const Line &other) const
{
    return !operator ==(other);
}

const Point *Line::meet(const Line &other) const
{
    constr_num det = x_coeff * other.y_coeff - y_coeff * other.x_coeff;

    // parallel lines
    if (det == 0)
        return nullptr;

    // might be neater with matrices
    constr_num x = (const_coeff * other.y_coeff - y_coeff * other.const_coeff) / det,
               y = (other.const_coeff * x_coeff - other.x_coeff * const_coeff) / det;

    Point* p = new Point(x, y);

    // check for boundaries for child classes
    if (within_boundary(*p) && other.within_boundary(*p))
        return p;
    return nullptr;
}

pair<const Point*,const Point*> Line::meet(const Circle &other) const
{
    return other.meet(*this);
}

bool Line::within_boundary(const Point &) const
{
    return true;
}

// Circle

Circle::Circle(const Point &center, constr_num radius) :
    center(center), radius(radius) {}

Circle::Circle(const Point &center, const Point &other) :
    center(center), radius(center.distance(other)) {}

constr_num Circle::distance(const Point &a) const
{
    return abs(sqrt(value_at(a) + radius * radius) -
               radius);
}

constr_num Circle::value_at(const Point &a) const
{
    return (a.x - center.x) * (a.x - center.x) +
           (a.y - center.y) * (a.y - center.y) -
           radius * radius;
}

bool Circle::contains(const Point &a) const
{
    return value_at(a) == 0 && within_boundary(a);
}

bool Circle::operator==(const Circle &other) const
{
    return center == other.center && radius == other.radius;
}

bool Circle::operator!=(const Circle &other) const
{
    return !operator ==(other);
}

pair<const Point*,const Point*> Circle::meet(const Line &other) const
{
    constr_num value = other.value_at(center),
               discr = radius * radius * other.norm() - value * value,
               x_offset = center.x + other.x_coeff * value / other.norm(),
               y_offset = center.y + other.y_coeff * value / other.norm();
    Point *p1 = nullptr, *p2 = nullptr;

    if (discr == 0) {
        p1 = new Point(x_offset, y_offset);
    } else if (discr > 0) {
        constr_num x_factor = other.y_coeff * sqrt(discr) / other.norm(),
                   y_factor = other.x_coeff * sqrt(discr) / other.norm();

        p1 = new Point(x_offset + x_factor,
                       y_offset - y_factor);
        p2 = new Point(x_offset - x_factor,
                       y_offset + y_factor);
    }

    return std::make_pair(p1, p2);
}

pair<const Point*,const Point*> Circle::meet(const Circle &other) const
{
    const Line l ((other.center.x - center.x) * 2,
                  (other.center.y - center.y) * 2,
                  value_at(*origin) - other.value_at(*origin));
    return meet(l);
}

bool Circle::within_boundary(const Point &) const
{
    return true;
}
