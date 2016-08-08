#include <include/constr_num.h>

void constr_num_test()
{
    constr_num a = 2;
    a = sqrt(a * 4 - 1);
    constr_num b = a;
    std::cout << b << " = " << a.value() << std::endl;
}
