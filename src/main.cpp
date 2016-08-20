#include <QCoreApplication>
#include <include/calculator.h>

using std::cout;
using std::endl;

#define TEST(obj) do { \
    void obj##_test(); \
    obj##_test();      \
} while (0)

int main(int argc, char *argv[])
{
    /*
    QCoreApplication a(argc, argv);

    return a.exec();
    */
    // TEST(constructor);

    // TODO make a small interpreter that allows the user to create objects (either assigned to variables or given default names)
    // and connect them through commands

    // TODO make these describe the objects connected and constructed
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

    Calculator c(&lis);

    while (1) {
        // prompt for input
        // parse input and execute command expressed
        // print moves made to the output, reporting errors as necessary
    }
}
