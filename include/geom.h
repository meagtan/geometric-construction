#ifndef GEOM_H
#define GEOM_H

#include "core.h"

// More geometry and operations by compass and straightedge construction

struct LineSegment : Line {
    Point start, end;

    LineSegment(const Point &start, const Point &end);

    bool operator==(const LineSegment &other) const;
};

struct Angle {
    // TODO how to represent an angle

    bool operator==(const Angle &other) const;
};

// stores useful compositions of elementary moves
class Constructor : public Scope {

    Constructor(MoveListener *listener);

    // include more operations on line segments and angles, such as bisection, intersection, perpendiculars

    // the perpendicular/parallel to l going through p
    Line *perpendicular(const Line &l, const Point &p);
    Line *parallel(const Line &l, const Point &p);

    // translate line segment by the given vector
    LineSegment *translate(const LineSegment &l, const Point &dx);

    // move the vertex of a to p
    Angle *translate(const Angle &a, const Point &p);

    LineSegment *rotate(const LineSegment &l, const Angle &a);

    Angle *add(const Angle &a, const Angle &b);
    Angle *multiply(const Angle &a, int n);

    Line *bisect(const LineSegment &l);
    Line *bisect(const Angle &l);
};

#endif // GEOM_H
