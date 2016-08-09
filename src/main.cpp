#include <QCoreApplication>

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
    TEST(angle);
}
