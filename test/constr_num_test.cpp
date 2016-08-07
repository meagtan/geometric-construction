#include <include/constr_num.h>

void constr_num_test()
{
    constr_num a = 2;
    a = sqrt(a * 4 + 1);
    std::cout << a << " = " << a.value() << std::endl;
}
