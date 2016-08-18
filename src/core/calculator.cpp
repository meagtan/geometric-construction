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

const Point *Calculator::get_add(constr_num a, constr_num b)
{
    const Point *pa = get_point(a),
                *pb = get_point(b),
                *p  = get_point(a + b);

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

const Point *Calculator::get_sub(constr_num a, constr_num b)
{
    const Point *pa = get_point(a),
                *pb = get_point(b),
                *p  = get_point(a - b);

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

const Point *Calculator::get_mul(constr_num a, constr_num b)
{
    const Point *pa = get_point(a),
                *pb = get_point(b, true),
                *p  = get_point(a * b);

    if (pa == nullptr || pb == nullptr)
        return nullptr;
    if (p != nullptr)
        return p;

    auto *l = join_line(*pa, *unit_y);
    assert(l != nullptr);

    return meet(*parallel(*l, *pb), *x_axis); // both l and pb are contained
}

const Point *Calculator::get_div(constr_num a, constr_num b)
{
    if (b == 0)
        return nullptr;

    const Point *pa = get_point(a),
                *pb = get_point(b, true),
                *p  = get_point(a / b);

    if (pa == nullptr || pb == nullptr)
        return nullptr;
    if (p != nullptr)
        return p;

    auto *l = join_line(*pa, *pb);
    assert(l != nullptr); // true only if a = b = 0

    return meet(*parallel(*l, *unit_y), *x_axis); // both l and unit_y are contained
}

const Point *Calculator::get_sqrt(constr_num a)
{
    const Point *pa    = get_point(a),
                *neg_x = get_point(-1),
                *p  = get_point(sqrt(a));

    if (pa == nullptr || a < 0)
        return nullptr;
    if (a == 0)
        return origin;
    if (p != nullptr)
        return p;

    auto *center = midpoint(*neg_x, *pa);
    assert(center != nullptr); // both neg_x and pa are contained

    auto meets = meet(*y_axis, *join_circle(*center, *pa)); // circle will always exist, as center != pa

    return meets.first->y < 0 ? meets.second : meets.first; // (0, √a)
}

const Point *Calculator::construct_number(constr_num n)
{
    const Point *p;

    switch (n.expr->type) {
    case 0:
        p = new Point(n.expr->expr_union.constant);
        addPoint(p);
        return p;
    case 1:
        switch (n.expr->expr_union.unary.op) {
        case 1:
            return get_sub(0, n.expr->expr_union.unary.arg);
        case 2:
            return get_div(1, n.expr->expr_union.unary.arg);
        default:
            return get_sqrt(n.expr->expr_union.unary.arg);
        }
    default:
        switch (n.expr->expr_union.binary.op) {
        case 1:
            return get_add(n.expr->expr_union.binary.arg1,
                           n.expr->expr_union.binary.arg2);
        default:
            return get_mul(n.expr->expr_union.binary.arg1,
                           n.expr->expr_union.binary.arg2);
        }
    }
}
