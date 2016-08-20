#include "include/cli.h"

using std::cout;
using std::endl;

// CLIProgram

CLIProgram::CLIProgram() : c(this) {}

CLIProgram::~CLIProgram() {}

void CLIProgram::input(string query)
{
    // the first word in query represents the command, the rest are arguments partitioned by semicolons
    // throw exception for parse error
}

// perhaps make these communicate with input() and wait until every name is assigned before printing

void CLIProgram::straightedge(const Point *p1, const Point *p2, const Line *l)
{
    string name = d.add(l);
    cout << name << " = straightedge " << d.get_name(p1) << "; " << d.get_name(p2) << endl;
}

void CLIProgram::compass(const Point *p1, const Point *p2, const Circle *c)
{
    string name = d.add(c);
    cout << name << " = compass " << d.get_name(p1) << "; " << d.get_name(p2) << endl;
}

void CLIProgram::meet(const Circle *c1, const Circle *c2, const Point *p)
{
    string name = d.add(p);
    cout << name << " = meet " << d.get_name(c1) << "; " << d.get_name(c2) << endl;
}

void CLIProgram::meet(const Line *l1, const Circle *c2, const Point *p)
{
    string name = d.add(p);
    cout << name << " = meet " << d.get_name(l1) << "; " << d.get_name(c2) << endl;
}

void CLIProgram::meet(const Line *l1, const Line *l2, const Point *p)
{
    string name = d.add(p);
    cout << name << " = meet " << d.get_name(l1) << "; " << d.get_name(l2) << endl;
}

// Shape

Shape::Shape(const struct Point *p) : type(Point)
{
    u.p = p;
}

Shape::Shape(const struct Line *l) : type(Line)
{
    u.l = l;
}

Shape::Shape(const struct Circle *c) : type(Circle)
{
    u.c = c;
}

Shape::Shape(const LineSegment *s) : type(Segment)
{
    u.s = s;
}

Shape::Shape(const struct Angle *a) : type(Angle)
{
    u.a = a;
}

Shape::Shape(const constr_num n) : type(Number)
{
    u.n = n;
}

Shape::Shape(const Shape &other) : type(other.type)
{
    switch (type) {
    case Point:
        u.p = other.p;
    case Line:
        u.l = other.l;
    case Circle:
        u.c = other.c;
    case Segment:
        u.s = other.s;
    case Angle:
        u.a = other.a;
    case Number:
        u.n = other.n;
    }
}

Shape::~Shape() {}

// Dictionary

