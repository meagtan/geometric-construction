#include <include/geom.h>

using std::cout;

void segment_test()
{
    Point a, b(3), c(3,4);
    LineSegment l(a, c);
    cout << l.length().value() << " = 5\n";
    cout << l.contains(Point(-3,-4)) << " = 0\n";
}

void angle_test()
{
    Point a, b(3), c(3,4);
    Angle a1(b, a, c), a2(c, a, b), a3(a, b, c);

    cout << a1.share_vertex(a2) << " but not " << a1.share_vertex(a3) << "\n";
    auto l = a1.shared_line(a2); // should be the line connecting a and b
    if (l != nullptr)
        cout << l->x_coeff.value() << " x + " << l->y_coeff.value() << " y + " <<
                l->const_coeff.value() << " = 0\n";

    cout << a1.sine().value() << "," << a2.sine().value() << "," << a3.sine().value() << "\n" <<
            a1.cosine().value() << "," << a2.cosine().value() << "," << a3.cosine().value() << '\n';
}
