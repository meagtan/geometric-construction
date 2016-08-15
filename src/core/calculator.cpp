#include "include/calculator.h"
#include <assert.h>

Calculator::Calculator(MoveListener *lis) : Constructor(lis) {}

Calculator::~Calculator() {}

const Point *Calculator::add(constr_num a, constr_num b)
{
    Point *pa = new Point(a),
          *pb = new Point(b);
    addPoint(pa);
    addPoint(pb);

    if (!contains(pa) || !contains(pb))
        return nullptr;

    auto *l = join_segment(*origin, *pb);
    if (l == nullptr) // b = 0
        return pa;

    l = translate(*l, *pa);
    assert(l != nullptr); // translate only returns null for arguments not contained

    return &l->end;
}

const Point *Calculator::sub(constr_num a, constr_num b)
{
    Point *pa = new Point(a),
          *pb = new Point(b);
    addPoint(pa);
    addPoint(pb);

    if (!contains(pa) || !contains(pb))
        return nullptr;

    auto *l = join_segment(*pb, *origin);
    if (l == nullptr) // b = 0
        return pa;

    l = translate(*l, *pa);
    assert(l != nullptr); // translate only returns null for arguments not contained

    return &l->end;
}
