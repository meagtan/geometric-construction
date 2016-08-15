#include "include/calculator.h"
#include <assert.h>

Calculator::Calculator(MoveListener *lis) : Constructor(lis) {}

Calculator::~Calculator() {}

const Point *Calculator::get_point(constr_num n, bool on_y_axis)
{
    Point *p;
    if (on_y_axis)
        p = new Point(0, n);
    else
        p = new Point(n);

    if (addPoint(p))
        return p;
    return nullptr;
}

const Point *Calculator::add(constr_num a, constr_num b)
{
    const Point *pa = get_point(a),
                *pb = get_point(b);

    if (pa == nullptr || pb == nullptr)
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
    const Point *pa = get_point(a),
                *pb = get_point(b);

    if (pa == nullptr || pb == nullptr)
        return nullptr;

    auto *l = join_segment(*pb, *origin);
    if (l == nullptr) // b = 0
        return pa;

    l = translate(*l, *pa);
    assert(l != nullptr); // translate only returns null for arguments not contained

    return &l->end;
}

const Point *Calculator::mul(constr_num a, constr_num b)
{
    const Point *pa = get_point(a),
                *pb = get_point(b, true);

    if (pa == nullptr || pb == nullptr)
        return nullptr;

    auto *l = join_line(*pa, *unit_y);
    assert(l != nullptr);

    return meet(*parallel(*l, *pb), *x_axis); // both l and pb are contained
}

const Point *Calculator::div(constr_num a, constr_num b)
{
    if (b == 0)
        return nullptr;

    const Point *pa = get_point(a),
                *pb = get_point(b, true);

    if (pa == nullptr || pb == nullptr)
        return nullptr;

    auto *l = join_line(*pa, *pb);
    assert(l != nullptr); // true only if a = b = 0

    return meet(*parallel(*l, *unit_y), *x_axis); // both l and unit_y are contained
}

const Point *Calculator::sqrt(constr_num a)
{
    const Point *pa    = get_point(a),
                *neg_x = get_point(-1);

    if (pa == nullptr || a < 0)
        return nullptr;
    if (a == 0)
        return origin;

    auto *center = midpoint(*neg_x, *pa);
    assert(center != nullptr); // both neg_x and pa are contained

    auto meets = meet(*y_axis, *join_circle(*center, *pa)); // circle will always exist, as center != pa

    return meets.first->y < 0 ? meets.second : meets.first; // (0, √a)
}

const Point *Calculator::construct_number(constr_num n)
{

}
