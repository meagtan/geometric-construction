#include <include/geom.h>

using std::cout;

void segment_test()
{
    Point a, b(3), c(3,4);
    LineSegment l(a, c);
    cout << l.length() << " = 5\n";
    cout << l.contains(Point(-3,-4)) << " = 0\n";
}

void angle_test()
{
    Point a, b(3), c(3,4);
    Angle a1(b, a, c), a2(c, a, b), a3(a, b, c);

    cout << a1.share_vertex(a2) << " but not " << a1.share_vertex(a3) << "\n";
    auto l = a1.shared_line(a2); // should be the line connecting a and b
    if (l != nullptr)
        cout << l->x_coeff << " x + " << l->y_coeff << " y + " <<
                l->const_coeff << " = 0\n";

    cout << a1.sine() << "," << a2.sine() << "," << a3.sine() << "\n" <<
            a1.cosine() << "," << a2.cosine() << "," << a3.cosine() << '\n';
}

void constructor_test()
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

    Constructor s(&lis);
    Point *a = new Point(),
          *b = new Point(3),
          *c = new Point(3, 4);
    s.addPoint(b);
    s.addPoint(c);
    auto *l = s.join_segment(*a, *c);
    cout << l->contains(*c) << " and " << s.contains(c) << '\n';
    auto *l1 = s.perpendicular(*l, *b);
    cout << l1->x_coeff.value() << " x + " << l1->y_coeff.value() << " y + " << l1->const_coeff.value() << " = 0\n";
    cout << l1->contains(*b) << " and not " << (l->x_coeff * l1->x_coeff + l->y_coeff * l1->y_coeff).value() << '\n';
}
