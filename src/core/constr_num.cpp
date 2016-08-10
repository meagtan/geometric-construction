#include "include/constr_num.h"
#include <cmath>

constr_num::constr_num(int value)
{
    expr = new Expr();
    expr->type = Expr::constant;
    expr->expr_union.constant = value;
}

constr_num::constr_num(Expr *expr) : expr(expr) {}

constr_num::constr_num(const constr_num &other)
{
    expr = copy(other.expr);
}

constr_num::Expr *constr_num::copy(Expr *expr) const
{
    if (expr == nullptr)
        return nullptr;

    Expr *res = new Expr();

    if (expr->type == Expr::constant) {
        res->type = Expr::constant;
        res->expr_union.constant = expr->expr_union.constant;
    } else if (expr->type == Expr::unary) {
        res->type = Expr::unary;
        res->expr_union.unary.op  = expr->expr_union.unary.op;
        res->expr_union.unary.arg = copy(expr->expr_union.unary.arg);
    } else {
        res->type = Expr::binary;
        res->expr_union.binary.op   = expr->expr_union.binary.op;
        res->expr_union.binary.arg1 = copy(expr->expr_union.binary.arg1);
        res->expr_union.binary.arg2 = copy(expr->expr_union.binary.arg2);
    }

    return res;
}

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
    return expr != nullptr && !std::fmod(value(), 1);
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
    if (isnan(arg))
        return NAN;

    if (op & 1) // neg
        return -arg;
    if (op & 2) // inv
        return arg == 0 ? NAN : 1 / arg;
    return arg < 0 ? NAN : sqrt(arg);
}

double constr_num::apply_binary(int op, double arg1, double arg2) const
{
    if (isnan(arg1) || isnan(arg2))
        return NAN;

    if (op & 1) // add
        return arg1 + arg2;
    return arg1 * arg2;
}

constr_num &constr_num::operator=(const constr_num &a)
{
    if (this != &a) {
        delete expr;
        expr = copy(a.expr);
    }

    return *this;
}

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

ostream &operator<<(ostream &s, const constr_num &a)
{
    a.print(s, a.expr);
    return s;
}

// quick hack, prints number as lisp expression
void constr_num::print(ostream &s, Expr *expr) const
{
    if (expr->type == Expr::constant) {
        s << expr->expr_union.constant;
    } else if (expr->type == Expr::unary) {
        s << '(';
        if (expr->expr_union.unary.op & 1)
            s << "- ";
        else if (expr->expr_union.unary.op & 4)
            s << "1/ ";
        else
            s << "sqrt ";
        print(s, expr->expr_union.unary.arg);
        s << ')';
    } else {
        s << "( " << (expr->expr_union.binary.op & 1 ? '+' : '*') << " ";
        print(s, expr->expr_union.binary.arg1);
        s << ' ';
        print(s, expr->expr_union.binary.arg2);
        s << ')';
    }
}

constr_num sqrt(const constr_num &a)
{
    constr_num::Expr *e = new constr_num::Expr();
    e->type = constr_num::Expr::unary;
    e->expr_union.unary.op = e->expr_union.unary.sqrt;
    e->expr_union.unary.arg = a.copy(a.expr);
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
