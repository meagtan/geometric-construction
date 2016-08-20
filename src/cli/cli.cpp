#include "include/cli.h"

// CLIProgram

CLIProgram::CLIProgram() : c(&lis) {}

CLIProgram::~CLIProgram() {}

void CLIProgram::input(string query)
{
    // the first word in query represents the command, the rest are arguments partitioned by semicolons
    // throw exception for parse error
}

// perhaps make these communicate with input() and wait until every name is assigned before printing

void CLIProgram::CLIListener::straightedge(const Point *p1, const Point *p2, const Line *l)
{
    string name = d.add(l);
    cout << name << " = straightedge " << d.get_name(p1) << "; " << d.get_name(p2) << endl;
}

void CLIProgram::CLIListener::compass(const Point *p1, const Point *p2, const Circle *c)
{
    string name = d.add(c);
    cout << name << " = compass " << d.get_name(p1) << "; " << d.get_name(p2) << endl;
}

void CLIProgram::CLIListener::meet(const Circle *c1, const Circle *c2, const Point *p)
{
    string name = d.add(p);
    cout << name << " = meet " << d.get_name(c1) << "; " << d.get_name(c2) << endl;
}

void CLIProgram::CLIListener::meet(const Line *l1, const Circle *c2, const Point *p)
{
    string name = d.add(p);
    cout << name << " = meet " << d.get_name(l1) << "; " << d.get_name(c2) << endl;
}

void CLIProgram::CLIListener::meet(const Line *l1, const Line *l2, const Point *p)
{
    string name = d.add(p);
    cout << name << " = meet " << d.get_name(l1) << "; " << d.get_name(l2) << endl;
}

// Shape

Shape::Shape(const Point *p) : type(Point)
{
    u.p = p;
}

Shape::Shape(const Line *l) : type(Line)
{
    u.l = l;
}

Shape::Shape(const Circle *c) : type(Circle)
{
    u.c = c;
}

Shape::Shape(const Angle *a) : type(Angle)
{
    u.a = a;
}

Shape::Shape(constr_num n) : type(Number)
{
    u.n = n;
}

Shape::~Shape() {}

// Dictionary

