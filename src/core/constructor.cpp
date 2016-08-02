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
    _make_move(straightedge, &a, Point, &b, Point, res, LineSegment);
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
