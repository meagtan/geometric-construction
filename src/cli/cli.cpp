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

    // don't pass this to command, pass a lambda or function pointer that can optionally set a name for the results of the command
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

// called from command, distinguished from the shapes added using the moves above
void CLIProgram::add(Shape shape)
{

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

SHAPE_CONSTR(struct Point, Point, p)
SHAPE_CONSTR(struct Line, Line, l)
SHAPE_CONSTR(struct Circle, Circle, c)
SHAPE_CONSTR(LineSegment, Segment, s)
SHAPE_CONSTR(struct Angle, Angle, a)
Shape::Shape(constr_num n) : type(Number), u((const struct Point *) nullptr), n(n) {}

#undef MEMBER_CONSTR
#undef SHAPE_CONSTR

Shape::Shape(const Shape &other) : type(other.type), u(other.u) {}

Shape::~Shape() {}

// Dictionary

Dictionary::Dictionary() {}
Dictionary::~Dictionary() {}

Shape *Dictionary::get_shape(string name)
{
    auto iter = shapes.find(name);
    if (iter != shapes.end())
        return &iter->second;
    return nullptr;
}

string Dictionary::get_name(Shape shape)
{
    return shape.name;
}

string Dictionary::add(Shape shape)
{
    // should also include when shape.name == ""
    if (shapes.find(shape.name) == shapes.end()) {
        shape.name = generate_name(shape.type);
        shapes.emplace(std::make_pair(shape.name, shape));
    }
    return shape.name;
}

string Dictionary::add(string name, Shape shape)
{
    if (shapes.find(shape.name) != shapes.end())
        shapes.erase(shape.name);
    shape.name = name;
    shapes.emplace(std::make_pair(name, shape));
    return name;
}

// TODO make this distinguish primary results of a command from other shapes, perhaps through names
string Dictionary::generate_name(int type)
{
    return letters[type] + std::to_string(counter[type]++);
}

// Command

Command::Command(vector<Shape::Type> args, CommandFunc fun)
    : args(args), fun(fun) {}
