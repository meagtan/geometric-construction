#include "include/geom.h"

LineSegment::LineSegment(const Point &start, const Point &end) :
    Line(start, end)
{
    auto minmax = std::minmax(start, end, this->precedes);
    start = minmax.first;
    end = minmax.end;
}

bool LineSegment::operator==(const LineSegment &other)
{
    return Line::operator==(other) &&
           ((this->start == other.start && this->end == other.end) ||
            (this->start == other.end && this->end == other.start));
}
