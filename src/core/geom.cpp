#include "include/geom.h"

// LineSegment

#define _min(a,b,sort) sort(a,b) ? a : b
#define _max(a,b,sort) sort(a,b) ? b : a

LineSegment::LineSegment(const Point &start, const Point &end) :
    Line(start, end),
    start(_min(start, end, Line::precedes)),
    end(_max(start, end, Line::precedes)) {}

#undef _min
#undef _max

const Point *LineSegment::shared_point(const LineSegment &other) const
{
    // assume this != other
    if (this->operator==(other))
        return nullptr;

    if (this->start == other.start || this->start == other.end)
        return &this->start;
    if (this->end == other.start || this->end == other.end)
        return &this->end;

    return nullptr;
}

bool LineSegment::operator==(const LineSegment &other) const
{
    return Line::operator==(other) &&
           ((this->start == other.start && this->end == other.end) ||
            (this->start == other.end && this->end == other.start));
}

bool LineSegment::within_boundary(const Point &a) const
{
    return this->precedes(this->start, a) &&
           this->precedes(a, this->end);
}

// Angle

#define _get_region(line, vertex, point) (line.precedes(vertex, point) ? 1 : -1)

Angle::Angle(const Point &end1, const Point &vertex, const Point &end2) :
    l1(*new Line(vertex, end1)), l2(*new Line(vertex, end2)), vertex(vertex),
    region1(_get_region(l1, vertex, end1)), region2(_get_region(l2, vertex, end2)) {}

#undef _get_region

Angle::Angle(const Line &l1, const Line &l2, int region1, int region2) :
    l1(l1), l2(l2), vertex(*l1.meet(l2)), region1(region1), region2(region2) {}

bool Angle::share_vertex(const Angle &other) const
{
    return this->vertex == other.vertex;
}

const Line *Angle::shared_line(const Angle &other) const
{
    if (!this->share_vertex(other))
        return nullptr;

    if (this->l1 == other.l1 || this->l1 == other.l2)
        return &this->l1;
    if (this->l2 == other.l1 || this->l2 == other.l2)
        return &this->l2;
    return nullptr;
}

constr_num Angle::sine() const
{
    return (this->l1.x_coeff * this->l2.y_coeff -
            this->l1.y_coeff * this->l2.x_coeff) /
           sqrt(this->l1.norm() * this->l2.norm());
}

constr_num Angle::cosine() const
{
    return (this->l1.x_coeff * this->l2.x_coeff +
            this->l1.y_coeff * this->l2.y_coeff) /
           sqrt(this->l1.norm() * this->l2.norm());
}

#define _try_side_combination(l1,l2,r1,r2) (this->l1 == other.l1 && this->l2 == other.l2 && this->r1 == other.r1 && this->r2 == other.r2)

bool Angle::operator==(const Angle &other) const
{
    return this->vertex == other.vertex &&
           (_try_side_combination(l1, l2, region1, region2) ||
            _try_side_combination(l2, l1, region2, region1));
}

#undef _try_side_combination
