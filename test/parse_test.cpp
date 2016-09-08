#include "include/cli.h"
using namespace std;

void parse_test()
{
    CLIProgram p;
    string s;
    constr_num num;

    while (true) {
        cout << "> ";
        getline(cin, s);
        if (p.parse_num(&num, s, 0, s.length()))
            cout << num << endl;
        else
            cout << "Invalid input." << endl;
    }
}
