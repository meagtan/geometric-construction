#ifndef GEOM_H
#define GEOM_H

#include "core.h"

// More geometry and operations by compass and straightedge construction

struct LineSegment : Line {
    const Point &start, &end;

    LineSegment(const Point &start, const Point &end);

    const Point *shared_point(const LineSegment &other) const;

    bool operator==(const LineSegment &other) const;

protected:
    bool within_boundary(const Point &a) const; // limit point to boundaries of line segment
};

struct Angle {
    const Line &l1, &l2;
    const Point &vertex;
    int region1, region2;

    Angle(const Point &end1, const Point &vertex, const Point &end2);
    Angle(const Line &l1, const Line &l2, int region1, int region2);

    bool share_vertex(const Angle &other) const;
    const Line *shared_line(const Angle &other) const;
    // perhaps more functions for intersections if the need arises

    constr_num sine() const; // sine of measure
    constr_num cosine() const; // cosine of measure

    bool operator==(const Angle &other) const;
};

// stores useful compositions of elementary moves
class Constructor : public Scope {

    Constructor(MoveListener *listener);

    // include more operations on line segments and angles, such as bisection, intersection, perpendiculars

    // the perpendicular/parallel to l going through p
    Line *perpendicular(const Line &l, const Point &p);
    Line *parallel(const Line &l, const Point &p);

    // translate line segment to the given starting point
    LineSegment *translate(const LineSegment &l, const Point &start);

    // move the vertex of a to p
    Angle *translate(const Angle &a, const Point &p);

    LineSegment *rotate(const LineSegment &l, const Angle &a);

    Angle *add(const Angle &a, const Angle &b);
    Angle *multiply(const Angle &a, int n);

    Line *bisect(const Point &a, const Point &b);
    Line *bisect(const LineSegment &l);
    Line *bisect(const Angle &a);
};

#endif // GEOM_H
