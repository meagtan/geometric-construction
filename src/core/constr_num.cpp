#include "include/constr_num.h"
#include <cmath>

constr_num::constr_num(int value)
{
    expr = constant(value);
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
        return arg == 0 ? NAN : (1 / arg);
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

void constr_num::print(ostream &s, Expr *expr, bool parenthesize) const
{
    if (expr->type == Expr::constant) {
        s << expr->expr_union.constant;
    } else if (expr->type == Expr::unary) {
        if (expr->expr_union.unary.op & 1)
            s << "-";
        else if (expr->expr_union.unary.op & 2)
            s << "1 / ";
        else
            s << "√";

        print(s, expr->expr_union.unary.arg, 1);
    } else {
        if (parenthesize) s << '(';

        // parenthesize addition within multiplication
        print(s, expr->expr_union.binary.arg1,
              expr->expr_union.binary.arg1->expr_union.binary.op <
              expr->expr_union.binary.op);

        // choose binary operator based on second argument
        if (expr->expr_union.binary.op & 1) { // addition
            // if the second argument is a negative constant or a negation, make it a subtraction instead of a sum
            if (expr->expr_union.binary.arg2->type == Expr::constant &&
                expr->expr_union.binary.arg2->expr_union.constant < 0) {
                s << " - " << -expr->expr_union.binary.arg2->expr_union.constant;
            } else if (expr->expr_union.binary.arg2->type == Expr::unary &&
                expr->expr_union.binary.arg2->expr_union.unary.op & 1) {
                s << " - ";
                // print negend of second argument, parenthesizing it if it is an addition
                print(s, expr->expr_union.binary.arg2->expr_union.unary.arg,
                      expr->expr_union.binary.arg2->expr_union.unary.arg->expr_union.binary.op & 1);
            } else {
                s << " + ";
                print(s, expr->expr_union.binary.arg2);
            }
        } else { // multiplication
            // if the second argument is an inversion
            if (expr->expr_union.binary.arg2->type == Expr::unary &&
                expr->expr_union.binary.arg2->expr_union.unary.op & 2) {
                s << " / ";
                // print argument of inversion, parenthesizing it if it is a binary expression
                print(s, expr->expr_union.binary.arg2->expr_union.unary.arg, 1);
            } else {
                s << " * ";
                // parenthesize addition within multiplication
                print(s, expr->expr_union.binary.arg2,
                      expr->expr_union.binary.arg2->expr_union.unary.op & 1);
            }
        }

        if (parenthesize) s << ')';
    }
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
