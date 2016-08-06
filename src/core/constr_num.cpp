#include "include/constr_num.h"
#include <math.h>

constr_num::constr_num(int value)
{
    expr = new Expr();
    expr->type = Expr::constant;
    expr->expr_union.constant = value;
}

constr_num::constr_num(Expr *expr) : expr(expr) {}

constr_num::~constr_num()
{
    delete expr;
}

constr_num::Expr::~Expr()
{
    if (type == Expr::unary)
        delete expr_union.unary.arg;
    if (type == Expr::binary) {
        delete expr_union.binary.arg1;
        delete expr_union.binary.arg2;
    }
}

bool constr_num::is_int() const
{
    return expr != nullptr && expr->type == Expr::constant;
}

double constr_num::value() const
{
    return value(expr);
}

double constr_num::value(Expr *expr) const
{
    if (expr->type == Expr::constant)
        return expr->expr_union.constant;
    if (expr->type == Expr::unary)
        return apply_unary(expr->expr_union.unary.op,
                           value(expr->expr_union.unary.arg));
    return apply_binary(expr->expr_union.binary.op,
                        value(expr->expr_union.binary.arg1),
                        value(expr->expr_union.binary.arg2));

}

double constr_num::apply_unary(int op, double arg) const
{
    if (op & 1) // neg
        return -arg;
    if (op & 2) // inv
        return 1 / arg;
    return sqrt(arg);
}

double constr_num::apply_binary(int op, double arg1, double arg2) const
{
    if (op & 1) // add
        return arg1 + arg2;
    return arg1 * arg2;
}

constr_num constr_num::operator-() const
{
    Expr *e = new Expr();
    e->type = Expr::unary;
    e->expr_union.unary.op = e->expr_union.unary.neg;
    e->expr_union.unary.arg = expr;
    return constr_num(e);
}

constr_num constr_num::operator+(const constr_num &a) const
{
    Expr *e = new Expr();
    e->type = Expr::binary;
    e->expr_union.binary.op = e->expr_union.binary.add;
    e->expr_union.binary.arg1 = expr;
    e->expr_union.binary.arg2 = a.expr;
    return constr_num(e);
}

constr_num constr_num::operator-(const constr_num &a) const
{
    return operator+(-a);
}

constr_num constr_num::operator*(const constr_num &a) const
{
    Expr *e = new Expr();
    e->type = Expr::binary;
    e->expr_union.binary.op = e->expr_union.binary.mul;
    e->expr_union.binary.arg1 = expr;
    e->expr_union.binary.arg2 = a.expr;
    return constr_num(e);
}

constr_num constr_num::operator/(const constr_num &a) const
{
    return operator*(a.inv());
}

constr_num constr_num::inv() const
{
    Expr *e = new Expr();
    e->type = Expr::unary;
    e->expr_union.unary.op = e->expr_union.unary.inv;
    e->expr_union.unary.arg = expr;
    return constr_num(e);
}

bool constr_num::operator==(const constr_num &a) const
{
    return value() == a.value();
}

bool constr_num::operator!=(const constr_num &a) const
{
    return value() != a.value();
}

bool constr_num::operator<(const constr_num &a) const
{
    return value() < a.value();
}

bool constr_num::operator>(const constr_num &a) const
{
    return value() > a.value();
}

bool constr_num::operator<=(const constr_num &a) const
{
    return value() <= a.value();
}

bool constr_num::operator>=(const constr_num &a) const
{
    return value() >= a.value();
}

constr_num sqrt(const constr_num &a)
{
    constr_num::Expr *e = new constr_num::Expr();
    e->type = constr_num::Expr::unary;
    e->expr_union.unary.op = e->expr_union.unary.sqrt;
    e->expr_union.unary.arg = a.expr;
    return constr_num(e);
}

constr_num abs(const constr_num &a)
{
    return a >= 0 ? a : -a;
}

int sgn(const constr_num &a)
{
    if (a > 0)
        return 1;
    if (a < 0)
        return -1;
    return 0;
}
