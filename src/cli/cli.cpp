#include "include/cli.h"

using std::cout;
using std::endl;

// CLIProgram

CLIProgram::CLIProgram() : c(this) {}

CLIProgram::~CLIProgram() {}

void CLIProgram::input(string query)
{
    // the first word in query represents the command, the rest are arguments partitioned by semicolons
    // the arguments can be either direct expressions to be parsed for numbers and points (call get_point) or variables
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

#define MEMBER_CONSTR(_type, _arg) \
    Shape::Member::Member(const _type *_arg) : _arg(_arg) {}
#define SHAPE_CONSTR(_class, _type, _arg) \
    Shape::Shape(const _class *_arg) : type(_type), u(_arg) {}

MEMBER_CONSTR(struct Point, p)
MEMBER_CONSTR(struct Line, l)
MEMBER_CONSTR(struct Circle, c)
MEMBER_CONSTR(LineSegment, s)
MEMBER_CONSTR(struct Angle, a)
MEMBER_CONSTR(constr_num, n)

SHAPE_CONSTR(struct Point, Point, p)
SHAPE_CONSTR(struct Line, Line, l)
SHAPE_CONSTR(struct Circle, Circle, c)
SHAPE_CONSTR(LineSegment, Segment, s)
SHAPE_CONSTR(struct Angle, Angle, a)
SHAPE_CONSTR(constr_num, Number, n)

#undef MEMBER_CONSTR
#undef SHAPE_CONSTR

/*
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
*/

Shape::~Shape() {}

// Dictionary

// Command

Command::Command(vector<Shape::Type> args, CommandFunc fun)
    : args(args), fun(fun) {}
