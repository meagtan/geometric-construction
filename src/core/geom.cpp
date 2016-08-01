#include "include/geom.h"

// LineSegment

#define min(a,b,sort) sort(a,b) ? a : b
#define max(a,b,sort) sort(a,b) ? b : a

LineSegment::LineSegment(const Point &start, const Point &end) :
    Line(start, end),
    start(min(start, end, Line::precedes)),
    end(max(start, end, Line::precedes)) {}

bool LineSegment::operator==(const LineSegment &other) const
{
    return Line::operator==(other) &&
           ((this->start == other.start && this->end == other.end) ||
            (this->start == other.end && this->end == other.start));
}

// TODO meet

// Angle

#define get_region(line, vertex, point) line.precedes(vertex, point) ? 1 : -1

Angle::Angle(const Point &end1, const Point &vertex, const Point &end2) :
    l1(*new Line(vertex, end1)), l2(*new Line(vertex, end2)), vertex(vertex),
    region1(get_region(l1, vertex, end1)), region2(get_region(l2, vertex, end2)) {}

Angle::Angle(const Line &l1, const Line &l2, int region1, int region2) :
    l1(l1), l2(l2), vertex(*l1.meet(l2)), region1(region1), region2(region2) {}
