#include "include/core.h"

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

void circle_test()
{
    Point a, b(3), c(3, 4);
    Circle c1(a, c);

    cout << "(x - " << c1.center.x.value() << ")^2 + (y - " << c1.center.y.value() << ")^2 = " << c1.radius.value() << "^2\n";
    cout << "(x,y) = (" << b.x.value() << "," << b.y.value() << ") => " << c1.value_at(b).value() << '\n';
    cout << c1.distance(b).value() << " = 2\n";
    cout << c1.contains(Point(-3, -4)) << " and " << c1.contains(c) << " but not " << c1.contains(b) << '\n';

    auto pair = c1.meet(Line(a, b));
    if (pair.first != nullptr && pair.second != nullptr)
        cout << "{" << pair.first->x.value() << "," << pair.second->x.value() << "} = {-5,5}\n";
    pair = c1.meet(Circle(b, a));
    if (pair.first != nullptr && pair.second != nullptr)
        cout << pair.first->x.value() << " = " << pair.second->x.value() << " but " <<
                pair.first->y.value() << " + " << pair.second->y.value() << " = 0\n";
}

void scope_test()
{
    struct MyListener : MoveListener {
        void straightedge(const Point*, const Point*, const Line*)
        {
            cout << "straightedge\n";
        }
        void compass(const Point*, const Point*, const Circle*)
        {
            cout << "compass\n";
        }
        void meet(const Circle*, const Circle*, const Point*)
        {
            cout << "meet circles\n";
        }
        void meet(const Line*, const Circle*, const Point*)
        {
            cout << "meet line and circle\n";
        }
        void meet(const Line*, const Line*, const Point*)
        {
            cout << "meet lines\n";
        }
    } lis;

    Point a(3, 4);
    Scope s(&lis);

    auto *l = s.join_line(a, *origin);
    if (l == nullptr) return;
    cout << s.contains(l) << " = 1\n";

    auto *c = s.join_circle(a, *s.meet(*l, *y_axis));
    if (c == nullptr) return;
    cout << c->radius.value() << " = 5\n";
}
