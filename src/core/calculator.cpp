#include "include/calculator.h"
#include <assert.h>

#define GET_POINT(n, on_y_axis) (on_y_axis) ? GET_POINT(n) : GET_POINT(0, n)

Calculator::Calculator(MoveListener *lis) : Constructor(lis) {}

Calculator::~Calculator() {}

const Point *Calculator::get_point(constr_num x, constr_num y)
{
    Point *p = new Point(x, y);

    if (addPoint(p))
        return p;
    delete p;
    return nullptr;
}

const Point *Calculator::get_add(constr_num a, constr_num b, bool on_y_axis)
{
    const Point *pa = GET_POINT(a, on_y_axis),
                *pb = GET_POINT(b, on_y_axis),
                *p  = GET_POINT(a + b, on_y_axis);

    if (pa == nullptr || pb == nullptr)
        return nullptr;
    if (p != nullptr)
        return p;

    auto *l = join_segment(*origin, *pb);
    if (l == nullptr) // b = 0
        return pa;

    l = translate(*l, *pa);
    assert(l != nullptr); // translate only returns null for arguments not contained

    return &l->end;
}

const Point *Calculator::get_sub(constr_num a, constr_num b, bool on_y_axis)
{
    const Point *pa = GET_POINT(a, on_y_axis),
                *pb = GET_POINT(b, on_y_axis),
                *p  = GET_POINT(a - b, on_y_axis);

    if (pa == nullptr || pb == nullptr)
        return nullptr;
    if (p != nullptr)
        return p;

    auto *l = join_segment(*pb, *origin);
    if (l == nullptr) // b = 0
        return pa;

    l = translate(*l, *pa);
    assert(l != nullptr); // translate only returns null for arguments not contained

    return &l->end;
}

const Point *Calculator::get_mul(constr_num a, constr_num b, bool on_y_axis)
{
    const Point *pa = GET_POINT(a, on_y_axis),
                *pb = GET_POINT(b, !on_y_axis),
                *p  = GET_POINT(a * b, on_y_axis);

    if (pa == nullptr || pb == nullptr)
        return nullptr;
    if (p != nullptr)
        return p;

    auto *l = join_line(*pa, on_y_axis ? *unit_x : *unit_y);
    assert(l != nullptr);

    return meet(*parallel(*l, *pb), on_y_axis ? *y_axis : *x_axis); // both l and pb are contained
}

const Point *Calculator::get_div(constr_num a, constr_num b, bool on_y_axis)
{
    if (b == 0)
        return nullptr;

    const Point *pa = GET_POINT(a, on_y_axis),
                *pb = GET_POINT(b, !on_y_axis),
                *p  = GET_POINT(a / b, on_y_axis);

    if (pa == nullptr || pb == nullptr)
        return nullptr;
    if (p != nullptr)
        return p;

    auto *l = join_line(*pa, *pb);
    assert(l != nullptr); // true only if a = b = 0

    return meet(*parallel(*l, on_y_axis ? *unit_x : *unit_y), on_y_axis ? *y_axis : *x_axis); // both l and unit_y are contained
}

const Point *Calculator::get_sqrt(constr_num a, bool on_y_axis)
{
    const Point *pa =  GET_POINT(a, on_y_axis),
                *neg = GET_POINT(-1, on_y_axis),
                *p   = GET_POINT(sqrt(a), on_y_axis);

    if (pa == nullptr || a < 0)
        return nullptr;
    if (a == 0)
        return origin;
    if (p != nullptr)
        return p;

    auto *center = midpoint(*neg, *pa);
    assert(center != nullptr); // both neg and pa are contained

    auto meets = meet(on_y_axis ? *x_axis : *y_axis, *join_circle(*center, *pa)); // circle will always exist, as center != pa

    return (on_y_axis ? meets.first->x : meets.first->y) < 0 ? meets.second : meets.first; // (0, √a)
}

const Point *Calculator::construct_number(constr_num n, bool on_y_axis)
{
    switch (n.expr->type) {
    case 0:
        return GET_POINT(n, on_y_axis);
    case 1:
        switch (n.expr->expr_union.unary.op) {
        case 1:
            return get_sub(0, n.expr->expr_union.unary.arg, on_y_axis);
        case 2:
            return get_div(1, n.expr->expr_union.unary.arg, on_y_axis);
        default:
            return get_sqrt(n.expr->expr_union.unary.arg, on_y_axis);
        }
    default:
        switch (n.expr->expr_union.binary.op) {
        case 1:
            return get_add(n.expr->expr_union.binary.arg1,
                           n.expr->expr_union.binary.arg2, on_y_axis);
        default:
            return get_mul(n.expr->expr_union.binary.arg1,
                           n.expr->expr_union.binary.arg2, on_y_axis);
        }
    }
}

const Point *Calculator::construct_point(const Point &p)
{
    const Point *px = construct_number(p.x),
                *py = construct_number(p.y, true);

    if (px == nullptr || py == nullptr)
        return nullptr;

    return meet(*parallel(*x_axis, *py),
                *parallel(*y_axis, *px));
}
