#ifndef CONSTR_NUM_H
#define CONSTR_NUM_H

// Constructible numbers
class constr_num {
    int value; // for test only, change later
public:
    constr_num(int value = 0);

    // TODO

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
