#include "include/constr_num.h"

constr_num::constr_num(int value)
{
    expr = new struct expr();
    expr->type = expr::constant;
    expr->expr_union.constant = value;
}

constr_num::constr_num(struct expr *expr)
{
    // TODO canonical form
}

constr_num::~constr_num()
{
    delete expr;
}

bool constr_num::is_int() const
{
    return expr != nullptr && expr->type == expr::constant;
}

constr_num constr_num::operator-() const
{
    return constr_num();
}

constr_num constr_num::operator+(const constr_num &a) const
{
    return constr_num();
}

constr_num constr_num::operator-(const constr_num &a) const
{
    return constr_num();
}

constr_num constr_num::operator*(const constr_num &a) const
{
    return constr_num();
}

constr_num constr_num::operator/(const constr_num &a) const
{
    return constr_num();
}

bool constr_num::operator==(const constr_num &a) const
{
    return false;
}

bool constr_num::operator!=(const constr_num &a) const
{
    return false;
}

bool constr_num::operator<(const constr_num &a) const
{
    return false;
}

bool constr_num::operator>(const constr_num &a) const
{
    return false;
}

bool constr_num::operator<=(const constr_num &a) const
{
    return false;
}

bool constr_num::operator>=(const constr_num &a) const
{
    return false;
}

constr_num sqrt(const constr_num &a)
{
    return constr_num();
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
