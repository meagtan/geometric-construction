#include "core.h"

// Point

Point::Point(constr_num x, constr_num y) : x(x), y(y) {}

constr_num Point::distance(const Point &other) const
{
    return sqrt(this->x * other.x + this->y * other.y);
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
    return this->value_at(a) == 0;
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
    return this->value_at(a) == 0;
}
