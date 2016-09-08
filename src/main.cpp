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
    TEST(parse);

    // TODO make a small interpreter that allows the user to create objects (either assigned to variables or given default names)
    // and connect them through commands

    /*
    CLIProgram p;
    string s;

    while (p.is_running()) {
        // prompt for input
        cout << "> ";
        std::getline(std::cin, s);

        p.input(s);
    }
    */
}
