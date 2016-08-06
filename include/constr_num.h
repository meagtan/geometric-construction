#ifndef CONSTR_NUM_H
#define CONSTR_NUM_H

#include <string>

using std::string;

// Constructible numbers
class constr_num {
    struct Expr {
        enum {
            constant,
            unary,
            binary
        } type;
        union {
            int constant;
            struct {
                enum {
                    neg  = 1,
                    inv  = 2,
                    sqrt = 4
                } op;
                Expr *arg;
            } unary;
            struct {
                enum {
                    add = 1,
                    mul = 2
                } op;
                Expr *arg1, *arg2;
            } binary;
        } expr_union;
        ~Expr();
    } *expr;

    constr_num(Expr *expr);

    double value(Expr *expr) const;
    double apply_unary(int op, double arg) const;
    double apply_binary(int op, double arg1, double arg2) const;

    constr_num inv() const;

    Expr *copy(Expr *expr) const;

public:
    constr_num(int value = 0);
    constr_num(string str);
    constr_num(const constr_num &other);
    ~constr_num();

    double value() const;

    bool is_int() const;

    constr_num &operator=(const constr_num &a);

    constr_num operator-() const;

    constr_num operator+(const constr_num &a) const;
    constr_num operator-(const constr_num &a) const;
    constr_num operator*(const constr_num &a) const;
    constr_num operator/(const constr_num &a) const;

    bool operator==(const constr_num &a) const;
    bool operator!=(const constr_num &a) const;
    bool operator<(const constr_num &a) const;
    bool operator>(const constr_num &a) const;
    bool operator<=(const constr_num &a) const;
    bool operator>=(const constr_num &a) const;

    friend constr_num sqrt(const constr_num &a);
    friend constr_num abs(const constr_num &a);
    friend int sgn(const constr_num &a);
};

#endif // CONSTR_NUM_H
