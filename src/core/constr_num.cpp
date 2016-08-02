#include "include/constr_num.h"

// Test implementation, will change later

constr_num::constr_num(int value) : value(value) {}

bool constr_num::is_int() const
{
    return true;
}

constr_num constr_num::operator-() const
{
    return constr_num(-value);
}

constr_num constr_num::operator+(const constr_num &a) const
{
    return constr_num(value + a.value);
}

constr_num constr_num::operator-(const constr_num &a) const
{
    return constr_num(value - a.value);
}

constr_num constr_num::operator*(const constr_num &a) const
{
    return constr_num(value * a.value);
}

constr_num constr_num::operator/(const constr_num &a) const
{
    return constr_num(value / a.value);
}

bool constr_num::operator==(const constr_num &a) const
{
    return value == a.value;
}

bool constr_num::operator!=(const constr_num &a) const
{
    return value != a.value;
}

bool constr_num::operator<(const constr_num &a) const
{
    return value < a.value;
}

bool constr_num::operator>(const constr_num &a) const
{
    return value > a.value;
}

bool constr_num::operator<=(const constr_num &a) const
{
    return value <= a.value;
}

bool constr_num::operator>=(const constr_num &a) const
{
    return value >= a.value;
}

constr_num sqrt(const constr_num &a)
{
    return constr_num(a.value);
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
