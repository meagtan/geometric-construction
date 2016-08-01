#include "include/core.h"

// Point

Point::Point(constr_num x, constr_num y) : x(x), y(y) {}

constr_num Point::distance(const Point &other) const
{
    return sqrt(this->x * other.x + this->y * other.y);
}

bool Point::operator==(const Point &other) const
{
    return this->x == other.x && this->y == other.y;
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
    return this->x_coeff * this->x_coeff +
           this->y_coeff * this->y_coeff;
}

constr_num Line::value_at(const Point &a) const
{
    return this->x_coeff * a.x + this->y_coeff * a.y + this->const_coeff;
}

constr_num Line::distance(const Point &a) const
{
    return abs(this->value_at(a)) / sqrt(this->norm());
}

bool Line::contains(const Point &a) const
{
    return this->value_at(a) == 0 && this->within_boundary(a);
}

bool Line::precedes(const Point &a, const Point &b) const
{
    return this->contains(a) && this->contains(b) &&
           this->x_coeff * (b.x - a.x) > 0;
}

bool Line::operator==(const Line &other) const
{
    return this->x_coeff * other.const_coeff == this->const_coeff * other.x_coeff &&
           this->y_coeff * other.const_coeff == this->const_coeff * other.y_coeff;
}

Point *Line::meet(const Line &other) const
{
    constr_num det = this->x_coeff * other.y_coeff - this->y_coeff * other.x_coeff;

    // parallel lines
    if (det == 0)
        return nullptr;

    // might be neater with matrices
    constr_num x = (this->const_coeff * other.y_coeff - this->y_coeff * other.const_coeff) / det,
               y = (other.const_coeff * this->x_coeff - other.x_coeff * this->const_coeff) / det;

    Point* p = new Point(x, y);

    // check for boundaries for child classes
    if (this->within_boundary(*p) && other.within_boundary(*p))
        return p;
    return nullptr;
}

pair<Point*,Point*> Line::meet(const Circle &other) const
{
    return other.meet(*this);
}

bool Line::within_boundary(const Point &a) const
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
    return abs(sqrt(this->value_at(a) + this->radius * this->radius) -
               this->radius);
}

constr_num Circle::value_at(const Point &a) const
{
    return (a.x - this->center.x) * (a.x - this->center.x) +
           (a.y - this->center.y) * (a.y - this->center.y) -
           this->radius * this->radius;
}

bool Circle::contains(const Point &a) const
{
    return this->value_at(a) == 0 && this->within_boundary(a);
}

bool Circle::operator==(const Circle &other) const
{
    return this->center == other.center && this->radius == other.radius;
}

pair<Point*,Point*> Circle::meet(const Line &other) const
{
    constr_num value = other.value_at(this->center),
               discr = this->radius * this->radius * other.norm() - value * value,
               x_offset = this->center.x + other.x_coeff * value / other.norm(),
               y_offset = this->center.y + other.y_coeff * value / other.norm();
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

pair<Point*,Point*> Circle::meet(const Circle &other) const
{
    Line l ((other.center.x - this->center.x) * 2,
            (other.center.y - this->center.y) * 2,
            this->value_at(origin) - other.value_at(origin));
    return this->meet(l);
}

bool Circle::within_boundary(const Point &a) const
{
    return true;
}

// Moves

template<typename T1, typename T2, typename R>
Move<T1,T2,R>::Move(MoveType move, T1 *arg1, T2 *arg2, R *result) :
    move(move), arg1(arg1), arg2(arg2), result(result) {}
