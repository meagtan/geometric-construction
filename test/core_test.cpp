#include <include/core.h>

using std::cout;

void point_test()
{
    Point a, b(3), c(3, 4);
    cout << a.distance(b).value() << "^2 + " << b.distance(c).value() << "^2 = " << a.distance(c).value() << "^2\n";
}

void line_test()
{
    Point a, b(3), c(3, 4);
    Line l(a, c), l2(b, c);

    cout << l.x_coeff.value() << " x + " << l.y_coeff.value() << " y + " << l.const_coeff.value() << " = 0\n";
    cout << l.value_at(b).value() << " / " << l.norm().value() << " = " << l.distance(b).value() << '\n';
    cout << l.contains(a) << " and " << l.contains(c) << " but not " << l.contains(b) << '\n';
    cout << l.precedes(a, c) << '\n';

    auto meet = l.meet(l2);
    if (meet != nullptr)
        cout << (*meet).x.value() << "," << (*meet).y.value() << '\n';
}
