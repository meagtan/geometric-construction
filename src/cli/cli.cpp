#include "include/cli.h"
#include <cctype>
#include <algorithm>

using std::cout;
using std::endl;

#define PRED(n) ((n) > 0 ? (n) - 1 : (n))

// CLIProgram

CLIProgram::CLIProgram() : c(this) {}

CLIProgram::~CLIProgram() {}

void CLIProgram::input(string query)
{
    int start, comm_end;
    Command comm;
    vector<Shape> args;
    Shape arg;

    // skip leading whitespace and find command
    for (start = 0; start != query.length() && isspace(query[start]); ++start);
    for (comm_end = start; comm_end != query.length() && !isspace(query[comm_end]); ++comm_end);

    auto comm_range = commands.equal_range(query.substr(start, comm_end));
    if (comm_range.first == commands.end()) { // TODO turn this into an exception
        cout << "Invalid input: command not found." << endl;
        return;
    }

    // skip whitespace
    for (; comm_end != query.length() && isspace(query[comm_end]); ++comm_end);

    // try each set of arguments viable for commands in comm_range
    // TODO clean this up and convert skips into conditionals
    for (auto pair = comm_range.first; pair != comm_range.second; ++pair) {
        comm = pair->second;
        args.clear();

        // skip for invalid number of arguments
        if (std::count(query.begin() + comm_end, query.end(), delim) != PRED(comm.args.size()))
            continue;

        // try to parse each argument based on its type
        for (int i = 0, pos = comm_end; i != comm.args.size(); ++i) {
            // parse argument
            arg = parse_arg(query.substr(pos, query.find(delim, pos)), comm.args[i]);

            // if argument cannot be parsed, skip to next command
            if (arg == null_shape)
                goto cont;

            args.push_back(arg);

            // move to after next delim and skip whitespace
            for (; comm_end != query.length() && query[comm_end] != delim; ++comm_end);
            for (; comm_end != query.length() && isspace(query[comm_end]); ++comm_end);
        }

        // apply args to command and exit
        (*comm.fun)(*this, c, args.data());
        return;

        cont:
    }

    // no matching command found, alert user
    cout << "Error: invalid set of arguments for command " << comm_range.first->first << "." << endl;
    cout << "The valid lists of arguments for " << comm_range.first->first << "are:" << endl;
    for (auto pair = comm_range.first; pair != comm_range.second; ++pair) {
        cout << '\t';
        for (auto arg = pair->second.args.begin(); arg != pair->second.args.end(); ++arg) {
            cout << types[*arg];
            if (arg != pair->second.args.end() - 1)
                cout << "; ";
        }
        cout << endl;
    }

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

void CLIProgram::quit() { running = false; }
bool CLIProgram::running() { return running; }

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

Shape::Shape() : Shape((const struct Point *) nullptr) {}
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

#define TYPE_CASE(_type, _arg) case _type: u._arg = other.u._arg; n = 0; break

Shape &Shape::operator=(const Shape &other)
{
    if (this != &other) {
        switch (other.type) {
            TYPE_CASE(Point, p);
            TYPE_CASE(Line, l);
            TYPE_CASE(Circle, c);
            TYPE_CASE(Segment, s);
            TYPE_CASE(Angle, a);
        default:
            u.p = nullptr;
            n = other.n;
        }
    }

    return *this;
}

#undef TYPE_CASE

#define TYPE_COND(_type, _arg) (type == _type && _arg == other._arg)

bool Shape::operator==(const Shape &other) const
{
    return type == other.type &&
           (TYPE_COND(Point, u.p)   ||
            TYPE_COND(Line, u.l)    ||
            TYPE_COND(Circle, u.c)  ||
            TYPE_COND(Segment, u.s) ||
            TYPE_COND(Angle, u.a)   ||
            TYPE_COND(Number, n));
}

#undef TYPE_COND

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
