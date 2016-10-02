#include "include/cli.h"

// CLIProgram

CLIProgram::CLIProgram() : c(this)
{
    cout << "The valid format for inputs is:\n    <command> [<arg>{; <arg>}]\nType help to see available commands.\n";
}

CLIProgram::~CLIProgram() {}

void CLIProgram::quit() { running = false; }
bool CLIProgram::is_running() { return running; }

void CLIProgram::help()
{
    printf("  The list of available commands are:\n    %-15sArguments\n", "Command");
    for (auto &pair : commands) {
        printf("    %-15s", pair.first.c_str());
        for (auto type : pair.second.args)
            printf("%-10s", types[type].c_str());
        cout << endl;
    }
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

void CLIProgram::add(Shape shape)
{
    cout << "Added " << d.get_name(shape) << endl;
    // TODO perhaps call print after this
}

void CLIProgram::print(const Point &p)
{
    cout << d.get_name(&p) << " = The point " << p << endl;
}

void CLIProgram::print(const Line &l)
{
    cout << d.get_name(&l) << " = The line " << l.x_coeff << " x + " << l.y_coeff << " y + " << l.const_coeff << " = 0" << endl;
}

void CLIProgram::print(const Circle &c)
{
    cout << d.get_name(&c) << " = The circle with center " << c.center << " and radius " << c.radius << endl;
}

void CLIProgram::print(const LineSegment &s)
{
    cout << d.get_name(&s) << " = The line segment between points " << s.start << " and " << s.end << endl;
}

void CLIProgram::print(const Angle &a)
{
    cout << d.get_name(&a) << " = The angle between lines \n\t" <<
            a.l1.x_coeff << " x + " << a.l1.y_coeff << " y + " << a.l1.const_coeff << " = 0 and \n\t" <<
            a.l1.x_coeff << " x + " << a.l1.y_coeff << " y + " << a.l1.const_coeff << " = 0" << endl;

}

void CLIProgram::print(constr_num n)
{
    cout << d.get_name(n) << " = " << n << endl;
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
Shape::Shape() : Shape((const struct Point *) nullptr) {}

#undef MEMBER_CONSTR
#undef SHAPE_CONSTR

Shape::~Shape() {}

#define TYPE_CASE(_type, _arg) case _type: u._arg = other.u._arg; n = 0; break

Shape::Shape(const Shape &other) : type(other.type), u(other.u)
{
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
        type = other.type;
    }

    return *this;
}

#undef TYPE_CASE

#define TYPE_COND(_type, _arg, ...) (type == _type && __VA_ARGS__ _arg == __VA_ARGS__ (other._arg))

bool Shape::operator==(const Shape &other) const
{
    return type == other.type &&
           (TYPE_COND(Point, u.p, *)   ||
            TYPE_COND(Line, u.l, *)    ||
            TYPE_COND(Circle, u.c, *)  ||
            TYPE_COND(Segment, u.s, *) ||
            TYPE_COND(Angle, u.a, *)   ||
            TYPE_COND(Number, n));
}

#undef TYPE_COND

// Dictionary

Dictionary::Dictionary()
{
    add("origin", origin);
    add("x_axis", x_axis);
    add("y_axis", y_axis);
    add(unit_x);
    add(unit_y);
}

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
    auto iter = std::find_if(shapes.begin(), shapes.end(), [shape](const pair<string,Shape> &p){return p.second == shape;});
    if (iter != shapes.end())
        return iter->first;
    if (shape.type == Shape::Point && auto_construct_ints && shape.u.p->x.is_int() && shape.u.p->y.is_int()) {
        string name = generate_name(shape);
        shapes.emplace(std::make_pair(name, shape));
        return name;
    }
    return "";
}

string Dictionary::add(Shape shape)
{
    string name = get_name(shape);
    if (name == "") {
        name = generate_name(shape);
        shapes.emplace(std::make_pair(name, shape));
    }
    return name;
}

string Dictionary::add(string name, Shape shape)
{
    string oldname = get_name(shape);
    if (oldname != "")
        shapes.erase(oldname);

    shapes.emplace(std::make_pair(name, shape));
    return name;
}

// TODO make this distinguish primary results of a command from other shapes, perhaps through names
string Dictionary::generate_name(Shape shape)
{
    int type = shape.type;
    std::stringstream s;

    if (type == Shape::Number && shape.n.is_int())
        s << shape.n;
    else if (type == Shape::Point && shape.u.p->x.is_int() && shape.u.p->y.is_int())
        s << *shape.u.p;
    else
        s << letters[type] << counter[type]++;
    return s.str();
}

// Command

Command::Command(vector<Shape::Type> args, CommandFunc fun)
    : args(args), fun(fun) {}
