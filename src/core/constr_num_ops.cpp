#include "include/constr_num.h"

// Arithmetic operations for constr_num

constr_num constr_num::operator-() const
{
    Expr *e = new Expr();
    e->type = Expr::unary;
    e->expr_union.unary.op = e->expr_union.unary.neg;
    e->expr_union.unary.arg = copy(expr);
    return constr_num(e);
}

constr_num constr_num::operator+(const constr_num &a) const
{
    Expr *e = new Expr();
    e->type = Expr::binary;
    e->expr_union.binary.op = e->expr_union.binary.add;
    e->expr_union.binary.arg1 = copy(expr);
    e->expr_union.binary.arg2 = copy(a.expr);
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
    e->expr_union.binary.arg1 = copy(expr);
    e->expr_union.binary.arg2 = copy(a.expr);
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
    e->expr_union.unary.arg = copy(expr);
    return constr_num(e);
}

constr_num sqrt(const constr_num &a)
{
    constr_num::Expr *e = new constr_num::Expr();
    e->type = constr_num::Expr::unary;
    e->expr_union.unary.op = e->expr_union.unary.sqrt;
    e->expr_union.unary.arg = a.copy(a.expr);
    return constr_num(e);
}
