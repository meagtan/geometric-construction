#ifndef CONSTR_NUM_H
#define CONSTR_NUM_H

// Constructible numbers
class constr_num {
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
};

constr_num sqrt(constr_num);
constr_num abs(constr_num);

#endif // CONSTR_NUM_H
