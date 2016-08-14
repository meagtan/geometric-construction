#include "include/constr_num.h"

void constr_num_test()
{
    constr_num a = 2;
    a = sqrt(a) / (sqrt(a) + 3);
    constr_num b = a;
    std::cout << b << " = " << a.value() << std::endl;
    // std::cout << a * a / 2 << " = 4\n";
}
