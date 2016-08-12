#include "include/constr_num.h"

// Arithmetic operations for constr_num

constr_num constr_num::operator-() const
{
    // if expr is a negation, extract negend
    // if expr is a constant, negate value
    // if expr is an addition, distribute negation across operands
    // else do the following
    Expr *e = new Expr();
    e->type = Expr::unary;
    e->expr_union.unary.op = e->expr_union.unary.neg;
    e->expr_union.unary.arg = copy(expr);
    return constr_num(e);
}

constr_num constr_num::operator+(const constr_num &a) const
{
    // if both arguments are constants, add them and return a new constant node
    // if expr is a negation and a.expr isn't, swap them
    // if a.expr is an addition, first add its first operand and then its second,
    //   to guarantee left associativity
    // else do the following
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
    // if both arguments are constants, add them and return a new constant node
    // if expr is an inversion and a.expr isn't, swap them
    // if either argument is an addition, distribute arguments
    // if a.expr is a multiplication, first add its first operand and then its second,
    //   to guarantee left associativity
    // else do the following
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
    // if expr is an inversion, extract its argument
    // if expr is a constant, invert its value (if possible)
    // if expr is a multiplication, distribute inversion across operands
    // else do the following
    Expr *e = new Expr();
    e->type = Expr::unary;
    e->expr_union.unary.op = e->expr_union.unary.inv;
    e->expr_union.unary.arg = copy(expr);
    return constr_num(e);
}

constr_num sqrt(const constr_num &a)
{
    // if a.expr is an inversion, invert the square root of the negend
    // if a.expr is a multiplication, distribute sqrt across its operands
    // else do the following
    constr_num::Expr *e = new constr_num::Expr();
    e->type = constr_num::Expr::unary;
    e->expr_union.unary.op = e->expr_union.unary.sqrt;
    e->expr_union.unary.arg = a.copy(a.expr);
    return constr_num(e);
}
