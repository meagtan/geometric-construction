#ifndef CONSTR_NUM_H
#define CONSTR_NUM_H

#include <iostream>

using std::ostream;

// Constructible numbers
class constr_num {
    friend class Calculator;

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

    // handle node creation, copying, canonical form internally
    Expr *constant(int value) const;
    Expr *negate(Expr *e) const;
    Expr *invert(Expr *e) const;
    Expr *_sqrt(Expr *e) const;
    Expr *add(Expr *e1, Expr *e2) const;
    Expr *mul(Expr *e1, Expr *e2) const;

    constr_num inv() const;

    Expr *copy(Expr *expr) const;

    void print(ostream &s, Expr *expr, bool parenthesize = 0) const; // parenthesize only concerns this being a binary expression

public:
    constr_num();
    constr_num(int value);
    constr_num(const constr_num &other);
    ~constr_num();

    double value() const;

    bool is_int() const;

    constr_num &operator=(const constr_num &a);

    friend constr_num operator-(const constr_num &);

    friend constr_num operator+(const constr_num &, const constr_num &);
    friend constr_num operator-(const constr_num &, const constr_num &);
    friend constr_num operator*(const constr_num &, const constr_num &);
    friend constr_num operator/(const constr_num &, const constr_num &);

    friend bool operator==(const constr_num &, const constr_num &);
    friend bool operator!=(const constr_num &, const constr_num &);
    friend bool operator<(const constr_num &, const constr_num &);
    friend bool operator>(const constr_num &, const constr_num &);
    friend bool operator<=(const constr_num &, const constr_num &);
    friend bool operator>=(const constr_num &, const constr_num &);

    friend ostream &operator<<(ostream &s, const constr_num &a);

    friend constr_num sqrt(const constr_num &a);
    friend constr_num abs(const constr_num &a);
    friend int sgn(const constr_num &a);
};

#endif // CONSTR_NUM_H
