#include "include/geom.h"
#include <assert.h>

Constructor::Constructor(MoveListener listener) :
    Scope(listener), angles() {}

bool Constructor::contains(const Angle *a) const
{
    return a != nullptr &&
           std::count_if(angles.begin(), angles.end(), [a](const Angle *p) {return *p == *a; });
}

void Constructor::add(const Angle *a)
{
    if (!contains(a))
        angles.push_back(a);
}

#define _make_move(movetype, arg1, arg1type, arg2, arg2type, res, restype) do {               \
    if (res != nullptr) {                                                                     \
        Scope::add(res);                                                                       \
        Scope::listener(Move<arg1type,arg2type,restype>(MoveType::movetype, arg1, arg2, res)); \
    }} while (0)

const LineSegment *Constructor::join_segment(const Point &a, const Point &b)
{
    if (!Scope::contains(&a) || !Scope::contains(&b))
        return nullptr;

    const LineSegment *res = new LineSegment(a, b);
    Scope::add(res);
    _make_move(straightedge, &a, Point, &b, Point, res, Line);
    return res;
}

const Angle *Constructor::join_angle(const Point &end1, const Point &vertex, const Point &end2)
{
    if (!Scope::contains(&end1) || !Scope::contains(&vertex) || !Scope::contains(&end2))
        return nullptr;

    const Angle *res = new Angle(end1, vertex, end2);
    add(res);
    if (!Scope::contains(&res->l1)) {
        Scope::add(&res->l1);
        _make_move(straightedge, &vertex, Point, &end1, Point, &res->l1, Line);
    }
    if (!Scope::contains(&res->l2)) {
        Scope::add(&res->l2);
        _make_move(straightedge, &vertex, Point, &end2, Point, &res->l2, Line);
    }
    return res;
}

#undef _make_move

const Line *Constructor::perpendicular(const Line &l, const Point &p)
{
    const Point *o1, *o2;
    const Circle *c, *c1, *c2;
    const Line &l1 (l); // ensure containment for line segments

    if (!Scope::contains(&p) || !Scope::contains(&l))
        return nullptr;

    // generate two points on the line equidistant from p

    o1 = nullptr;
    for (auto *p2 : points)
        if (l1.contains(*p2) && *p2 != p) { o1 = p2; break; }
    assert(o1 != nullptr);

    c = join_circle(p, *o1);
    assert(c != nullptr);

    auto pair = meet(l1, *c); // the copy constructor works well with Scope::contains
    o2 = pair.first == o1 ? pair.second : pair.first;
    assert(o2 != nullptr);

    // create circles from o1 to o2 and from o2 to o1

    c1 = join_circle(*o1, *o2);
    c2 = join_circle(*o2, *o1);
    assert(c1 != nullptr);
    assert(c2 != nullptr);
    assert(*c1 != *c2); // o1 and o2 are different

    pair = meet(*c1, *c2);
    assert(pair.first != nullptr);
    assert(pair.second != nullptr);
    assert(*pair.first != *pair.second); // c1 and c2 are different

    return join_line(*pair.first, *pair.second);

    // the deletion of all these items are to be handled by ~Scope()
}

const Line *Constructor::parallel(const Line &l, const Point &p)
{
    if (l.contains(p))
        return &l;

    auto perp = perpendicular(l, p);
    if (perp == nullptr)
        return nullptr;

    return perpendicular(*perp, p);
}

const LineSegment *Constructor::translate(const LineSegment &l, const Point &start)
{
    const Line *par, *diff, *diff1;
    const Point *end;
    const LineSegment *res;

    par = parallel(l, start);
    if (par == nullptr) // l or start isn't added
        return nullptr;

    diff = join_line(l.start, start);
    assert(diff != nullptr);

    diff1 = parallel(*diff, l.end);
    assert(diff1 != nullptr);

    end = meet(*par, *diff1);
    assert(end != nullptr); // TODO move these assertions to Scope

    res = new LineSegment(start, *end);
    Scope::add(res);
    return res;
}

#define _ratio(l1, l2) ((l1).x_coeff == 0) ? sgn((l1).y_coeff * (l2).y_coeff) : sgn((l1).x_coeff * (l2).x_coeff)

const Angle *Constructor::translate(const Angle &a, const Point &p)
{
    const Line *l1 = parallel(a.l1, p),
               *l2 = parallel(a.l2, p);
    // the sign of the ratio of nonconstant coefficients for l1 and l2
    int r1 = _ratio(a.l1, *l1),
        r2 = _ratio(a.l2, *l2);

    const Angle *res = new Angle(*l1, *l2, r1 * a.region1, r2 * a.region2);
    add(res);
    return res;
}

#undef _ratio

const Line *Constructor::bisect(const Point &a, const Point &b)
{
    const Circle *c1 = join_circle(a, b),
                 *c2 = join_circle(b, a);

    if (c1 == nullptr || c2 == nullptr || c1 == c2)
        return nullptr;
    auto _meet = meet(*c1, *c2);
    assert(_meet.first != nullptr);
    assert(_meet.second != nullptr);

    return join_line(*_meet.first, *_meet.second);
}

const Line *Constructor::bisect(const LineSegment &l)
{
    return bisect(l.start, l.end);
}

const Line *Constructor::bisect(const Angle &a)
{
    if (a.l1 == a.l2)
        return &a.l1;

    // find point on l1 other than vertex
    const Point *p1 = nullptr;
    for (auto *p : points)
        if (a.l1.contains(*p) && *p != a.vertex) { p1 = p; break; }
    assert(p1 != nullptr);

    // find point on l2 with same region as p1
    const Circle *c = join_circle(a.vertex, *p1);
    auto _meet = meet(a.l2, *c);
    assert(_meet.first != nullptr);
    assert(_meet.second != nullptr);

    const Point *p2 = a.l1.precedes(a.vertex, *p1) == a.l1.precedes(a.vertex, *_meet.first) ? _meet.first : _meet.second;
    return join_line(*p1, *p2);
}

const Point *Constructor::reflect(const Point &a, const Point &pivot)
{
    const Line *l = join_line(a, pivot);
    const Circle *c = join_circle(pivot, a);
    auto _meet = meet(*l, *c);

    if (_meet.first == nullptr || _meet.second == nullptr)
        return nullptr;

    return *_meet.first == a ? _meet.second : _meet.first;
}

const Point *Constructor::reflect(const Point &a, const Line &pivot)
{
    const Line &p (pivot), // override within_boundary
               *l = perpendicular(p, a);

    if (l == nullptr)
        return nullptr;
    const Point *c = meet(p, *l);

    if (c == nullptr)
        return nullptr;
    return reflect(a, *c);
}

const Line *Constructor::reflect(const Line &a, const Point &pivot)
{
    const Line &l (a),
               *p = perpendicular(l, pivot);

    if (p == nullptr)
        return nullptr;
    const Point *b = reflect(*meet(l, *p), pivot);

    if (b == nullptr)
        return nullptr;
    return perpendicular(*p, *b);
}

const Line *Constructor::reflect(const Line &a, const Line &pivot)
{
    // different cases for a and pivot being parallel or not
    const Point *vertex = meet(a, pivot), *p1 = nullptr;

    if (vertex == nullptr) { // parallel
        // find point on pivot
        for (auto *p : points)
            if (pivot.contains(*p)) { p1 = p; break; }
        assert(p1 != nullptr);

        // reflect a by p1
        return reflect(a, *p1);
    }
    // not parallel

    // find point on a
    for (auto *p : points)
        if (a.contains(*p) && *p != *vertex) { p1 = p; break; }
    assert(p1 != nullptr);

    // reflect p1 around pivot
    const Point *p = reflect(*p1, pivot);

    if (p == nullptr)
        return nullptr;
    return join_line(*vertex, *p);
}

const Line *Constructor::rotate(const Line &l, const Angle &a, const Point &pivot)
{
    // find bisector of angle
    const Line *l1 = bisect(a);
    assert(l1 != nullptr);

    // get parallel of bisector at pivot
    l1 = parallel(*l1, pivot);
    assert(l1 != nullptr);

    // reflect line around parallel
    return reflect(l, *l1);
}

const LineSegment *Constructor::rotate(const LineSegment &l, const Angle &a)
{
    // find bisector of angle
    const Line *l1 = bisect(a);
    if (l1 == nullptr) return nullptr;

    // get parallel of bisector at l.start
    l1 = parallel(*l1, l.start);
    if (l1 == nullptr) return nullptr;

    // reflect end around parallel
    const Point *end = reflect(l.end, *l1);
    if (end == nullptr) return nullptr;

    // connect start and end
    return join_segment(l.start, *end);

    /* Awful previous algorithm:
    // - Translate l to the vertex of a, call the new segment l1
    const LineSegment *l1 = translate(l, a.vertex);

    // - Draw circle with center l1.start and touching l1.end
    if (l1 == nullptr)
        return nullptr;
    const Circle *c = join_circle(l1->start, l1->end);

    // - Pick the meets p1 and p2 of the circle with a.l1 and a.l2 that lie in a.region1 and a.region2
    auto meet1 = meet(a.l1, *c),
         meet2 = meet(a.l2, *c);
    assert(meet1.second != nullptr);
    assert(meet2.second != nullptr);
    const Point *p1 = a.l1.precedes(a.vertex, *meet1.first) == (a.region1 > 0) ? meet1.first : meet1.second,
                *p2 = a.l2.precedes(a.vertex, *meet2.first) == (a.region2 > 0) ? meet2.first : meet2.second;

    // - Find perpendicular bisector to l1.end and p2
    const Line *l2 = bisect(l1->end, *p2);
    assert(l2 != nullptr);

    // - Reflect p1 around the bisector
    const Point *p = reflect(*p1, *l2);
    assert(p != nullptr);

    // - Connect l1.start with p1 and translate the new line segment back to l.start
    const LineSegment *l3 = join_segment(l1->start, *p);
    assert(l3 != nullptr);

    return translate(*l3, l.start);
    */
}
