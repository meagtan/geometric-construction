#include <QCoreApplication>
#include "include/cli.h"

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

    CLIProgram p;
    string s;

    while (p.running()) {
        // prompt for input
        cout << "> ";
        std::getline(cin, s);

        p.input(s);
    }
}
