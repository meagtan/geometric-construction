#include "include/geom.h"
#include <assert.h>

Constructor::Constructor(MoveListener listener) :
    Scope(listener) {}

#define set_point(p1, l, p) do { \
    p1 = nullptr; \
    for (auto *p2 : points) \
        if (l.contains(*p2) && *p2 != p) { p1 = p2; break; } \
    assert(p1 != nullptr); \
} while (0)

const Line *Constructor::perpendicular(const Line &l, const Point &p)
{
    const Point *o1, *o2;
    const Circle *c, *c1, *c2;
    const Line &l1 (l); // ensure containment for line segments

    if (!contains(&p) || !contains(&l))
        return nullptr;

    // generate two points on the line equidistant from p

    set_point(o1, l1, p);

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

#undef set_point

const Line *Constructor::parallel(const Line &l, const Point &p)
{
    if (l.contains(p))
        return &l;

    auto perp = perpendicular(l, p);
    if (perp == nullptr)
        return nullptr;

    return perpendicular(*perp, p);
}
