#ifndef GEOM_H
#define GEOM_H

#include "core.h"

// More geometry and operations by compass and straightedge construction

struct LineSegment : Line {
    const Point &start, &end;

    LineSegment(const Point &start, const Point &end);
    ~LineSegment() = default;

    const Point *shared_point(const LineSegment &other) const;

    bool operator==(const LineSegment &other) const;

    constr_num length() const;

protected:
    bool within_boundary(const Point &a) const; // limit point to boundaries of line segment
};

struct Angle {
    const Line &l1, &l2;
    const Point &vertex;
    int region1, region2;

    Angle(const Point &end1, const Point &vertex, const Point &end2);
    Angle(const Line &l1, const Line &l2, int region1, int region2);
    ~Angle() = default;

    bool share_vertex(const Angle &other) const;
    const Line *shared_line(const Angle &other) const;
    // perhaps more functions for intersections if the need arises

    constr_num sine() const; // sine of measure
    constr_num cosine() const; // cosine of measure

    bool operator==(const Angle &other) const;
};

// stores useful compositions of elementary moves
class Constructor : public Scope {
protected:
    vector<const Angle*> angles;
    using Scope::add;
    void add(const Angle *a);

public:
    Constructor(MoveListener *listener);
    virtual ~Constructor();

    using Scope::contains;
    bool contains(const Angle *a) const;

    const LineSegment *join_segment(const Point &a, const Point &b);
    const Angle *join_angle(const Point &end1, const Point &vertex, const Point &end2);

    // the perpendicular/parallel to l going through p
    const Line *perpendicular(const Line &l, const Point &p);
    const Line *parallel(const Line &l, const Point &p);

    // translate line segment to the given starting point
    const LineSegment *translate(const LineSegment &l, const Point &start);

    // move the vertex of a to p
    const Angle *translate(const Angle &a, const Point &p);

    const Line *bisect(const Point &a, const Point &b); // perpendicular bisector between a and b
    const Line *bisect(const LineSegment &l); // perpendicular bisector of l
    const Line *bisect(const Angle &a); // bisector of the angle going through the vertex

    const Point *midpoint(const Point &a, const Point &b);
    const Point *midpoint(const LineSegment &a);

    const Point *reflect(const Point &a, const Point &pivot);
    const Point *reflect(const Point &a, const Line &pivot);
    const Line *reflect(const Line &a, const Point &pivot);
    const Line *reflect(const Line &a, const Line &pivot);

    /*
    const Line *rotate(const Line &l, const Angle &a, const Point &pivot);
    const LineSegment *rotate(const LineSegment &l, const Angle &a);

    const Angle *add(const Angle &a, const Angle &b);
    const Angle *multiply(const Angle &a, int n);
    */
};

#endif // GEOM_H
