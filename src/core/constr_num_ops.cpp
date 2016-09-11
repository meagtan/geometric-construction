#include "include/constr_num.h"
#include <cmath>

#define IS_CONSTANT(ptr_to_expr)  ((ptr_to_expr)->type == constr_num::Expr::constant)
#define IS_NEGATION(ptr_to_expr)  ((ptr_to_expr)->type == constr_num::Expr::unary  && ((ptr_to_expr)->expr_union.unary.op & 1))
#define IS_INVERSION(ptr_to_expr) ((ptr_to_expr)->type == constr_num::Expr::unary  && ((ptr_to_expr)->expr_union.unary.op & 2))
#define IS_SQRT(ptr_to_expr)      ((ptr_to_expr)->type == constr_num::Expr::unary  && ((ptr_to_expr)->expr_union.unary.op & 4))
#define IS_ADDITION(ptr_to_expr)  ((ptr_to_expr)->type == constr_num::Expr::binary && ((ptr_to_expr)->expr_union.binary.op & 1))
#define IS_PRODUCT(ptr_to_expr)   ((ptr_to_expr)->type == constr_num::Expr::binary && ((ptr_to_expr)->expr_union.binary.op & 2))

// Arithmetic operations for constr_num

constr_num constr_num::operator-() const
{
    return constr_num(negate(expr));
}

constr_num constr_num::operator+(const constr_num &a) const
{
    return constr_num(add(expr, a.expr));
}

constr_num constr_num::operator-(const constr_num &a) const
{
    return operator+(-a);
}

constr_num constr_num::operator*(const constr_num &a) const
{
    return constr_num(mul(expr, a.expr));
}

constr_num constr_num::operator/(const constr_num &a) const
{
    return operator*(a.inv());
}

constr_num constr_num::inv() const
{
    return constr_num(invert(expr));
}

constr_num sqrt(const constr_num &a)
{
    return constr_num(a._sqrt(a.expr));
}

constr_num::Expr *constr_num::constant(int value) const
{
    Expr *e = new Expr();
    e->type = Expr::constant;
    e->expr_union.constant = value;
    return e;
}

constr_num::Expr *constr_num::negate(Expr *arg) const
{
    // if arg is a negation, extract negend
    if (IS_NEGATION(arg))
        return copy(arg->expr_union.unary.arg);

    // if arg is a constant, negate its value
    if (IS_CONSTANT(arg))
        return constant(-arg->expr_union.constant);

    // if arg is an addition, distribute negation across operands
    if (IS_ADDITION(arg))
        return add(negate(arg->expr_union.binary.arg1),
                   negate(arg->expr_union.binary.arg2));

    // else create new negation node
    Expr *e = new Expr();
    e->type = Expr::unary;
    e->expr_union.unary.op = e->expr_union.unary.neg;
    e->expr_union.unary.arg = copy(arg);
    return e;
}

constr_num::Expr *constr_num::invert(Expr *arg) const
{
    // if arg is an inversion, extract argument
    if (IS_INVERSION(arg))
        return copy(arg->expr_union.unary.arg);

    // if arg is a product, distribute inversion across operands
    if (IS_PRODUCT(arg))
        return mul(invert(arg->expr_union.binary.arg1),
                   invert(arg->expr_union.binary.arg2));

    // else create new inversion node
    Expr *e = new Expr();
    e->type = Expr::unary;
    e->expr_union.unary.op = e->expr_union.unary.inv;
    e->expr_union.unary.arg = copy(arg);
    return e;
}

constr_num::Expr *constr_num::_sqrt(Expr *arg) const
{
    // if arg is an inversion, invert the square root of its argument
    if (IS_INVERSION(arg))
        return mul(_sqrt(arg->expr_union.unary.arg),
                   invert(arg->expr_union.unary.arg));
    // TODO check for square numbers and factors

    /*
    // if arg is a multiplication, distribute sqrt across its operands
    if (IS_PRODUCT(arg))
        return mul(_sqrt(arg->expr_union.binary.arg1),
                   _sqrt(arg->expr_union.binary.arg2));
    */

    // else create new sqrt node
    Expr *e = new Expr();
    e->type = Expr::unary;
    e->expr_union.unary.op = e->expr_union.unary.sqrt;
    e->expr_union.unary.arg = copy(arg);
    return e;
}

constr_num::Expr *constr_num::add(Expr *e1, Expr *e2) const
{
    // if both arguments are constants, add them and return a new constant node
    if (IS_CONSTANT(e1) && IS_CONSTANT(e2))
        return constant(e1->expr_union.constant + e2->expr_union.constant);

    // when an argument is 0
    if (IS_CONSTANT(e1) && e1->expr_union.constant == 0)
        return copy(e2);
    if (IS_CONSTANT(e2) && e2->expr_union.constant == 0)
        return copy(e1);

    // if e1 is a negation and e2 isn't, swap them
    if (IS_NEGATION(e1) && !IS_NEGATION(e2)) {
        Expr *temp = e1;
        e1 = e2;
        e2 = temp;
    }

    // if e2 is an addition, first add its first operand and then its second,
    //   to guarantee left associativity
    if (IS_ADDITION(e2))
        return add(add(e1, e2->expr_union.binary.arg1), e2->expr_union.binary.arg2);

    // else create new node
    Expr *e = new Expr();
    e->type = Expr::binary;
    e->expr_union.binary.op = e->expr_union.binary.add;
    e->expr_union.binary.arg1 = copy(e1);
    e->expr_union.binary.arg2 = copy(e2);
    return e;
}

constr_num::Expr *constr_num::mul(Expr *e1, Expr *e2) const
{
    // if both arguments are constants, multiply them and return a new constant node
    if (IS_CONSTANT(e1) && IS_CONSTANT(e2))
        return constant(e1->expr_union.constant * e2->expr_union.constant);

    // when an argument is 0
    if ((IS_CONSTANT(e1) && e1->expr_union.constant == 0) ||
        (IS_CONSTANT(e2) && e2->expr_union.constant == 0))
        return constant(0);

    // when an argument is 1
    if (IS_CONSTANT(e1) && e1->expr_union.constant == 1)
        return copy(e2);
    if (IS_CONSTANT(e2) && e2->expr_union.constant == 1)
        return copy(e1);

    // detect squares of square roots
    if (IS_SQRT(e1) && IS_SQRT(e2) &&
        value(e1->expr_union.unary.arg) == value(e2->expr_union.unary.arg))
        return copy(e1->expr_union.unary.arg);

    // propagate negations outward
    if (IS_NEGATION(e1) || (IS_CONSTANT(e1) && value(e1) < 0))
        return negate(mul(negate(e1), e2));
    if (IS_NEGATION(e2) || (IS_CONSTANT(e2) && value(e2) < 0))
        return negate(mul(e1, negate(e2)));


    // if e1 is an inversion and e2 isn't, swap them
    if (IS_INVERSION(e1) && !IS_INVERSION(e2)) {
        Expr *temp = e1;
        e1 = e2;
        e2 = temp;
    }

    // if e2 is a multiplication, first multiply its first operand and then its second,
    //   to guarantee left associativity
    if (IS_PRODUCT(e2))
        return mul(mul(e1, e2->expr_union.binary.arg1), e2->expr_union.binary.arg2);

    // else create new node
    Expr *e = new Expr();
    e->type = Expr::binary;
    e->expr_union.binary.op = e->expr_union.binary.mul;
    e->expr_union.binary.arg1 = copy(e1);
    e->expr_union.binary.arg2 = copy(e2);
    return e;
}
