#ifndef CONSTR_NUM_H
#define CONSTR_NUM_H

// Constructible numbers
class constr_num {
    struct expr {
        enum {
            constant,
            unary,
            binary
        } type;
        union {
            int constant;
            struct {
                enum {
                    neg,
                    inv,
                    sqrt
                } op;
                expr *arg;
            } unary;
            struct {
                enum {
                    add,
                    mul
                } op;
                expr *arg1, *arg2;
            } binary;
        } expr_union;
    } *expr;

    constr_num(struct expr *expr);

public:
    constr_num(int value = 0);
    ~constr_num();

    double value() const;

    bool is_int() const;

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
