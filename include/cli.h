#ifndef CLI_H
#define CLI_H

#include "calculator.h"
#include <string>
#include <map>
#include <cstdarg>

using std::string;
using std::map;
using std::multimap;

struct Shape {
    const enum Type {
        Point,
        Line,
        Circle,
        Segment, // TODO implement bit basis for Type to make inheritance work
        Angle,
        Number
    } type;
    const union {
        const struct Point  *p;
        const struct Line   *l;
        const struct Circle *c;
        const LineSegment *s;
        const struct Angle  *a;
        constr_num n;
    } u;

    Shape(const struct Point  *);
    Shape(const struct Line   *);
    Shape(const struct Circle *);
    Shape(const LineSegment   *);
    Shape(const struct Angle  *);
    Shape(const constr_num);
    Shape(const Shape &);
    ~Shape(); // does not delete pointer
};

struct Dictionary {
    map<string,Shape> shapes;

    Dictionary();
    ~Dictionary();

    Shape get_shape(string name);
    string get_name(Shape name);

    // automatically coerces pointers taken as arguments
    string add(Shape shape); // assigns default name to shape
    string add(string name, Shape shape); // assigns given name to shape if not already assigned, in which case assigns default name
};

struct Command {
    vector<Shape::Type> args;
    Shape (*fun)(Calculator &, Shape...);

    Command(vector<Shape::Type> args, Shape (*fun)(Calculator &, Shape...));
    ~Command() = default;
};

class CLIProgram : private MoveListener {
    friend class Calculator;

    void straightedge(const Point*, const Point*, const Line*);
    void compass(const Point*, const Point*, const Circle*);
    void meet(const Circle*, const Circle*, const Point*);
    void meet(const Line*, const Circle*, const Point*);
    void meet(const Line*, const Line*, const Point*);

    Calculator c;
    Dictionary d;

public:
    CLIProgram();
    ~CLIProgram();

    void input(string query);
};

#define UNARY_COMMAND_OBJ(command, type, arg) \
    {{Shape::Type::type}, [](Calculator &c, Shape shape, ...) { return c.command(*shape.u.arg); }}
#define BINARY_COMMAND_OBJ(command, type1, arg1, type2, arg2) \
    {{Shape::Type::type1, Shape::Type::type2}, \
     [](Calculator &c, Shape shape1, Shape shape2, ...) { return c.command(*shape1.u.arg1, *shape2.u.arg2); }}
#define TERNARY_COMMAND_OBJ(command, type1, arg1, type2, arg2, type3, arg3) \
    {{Shape::Type::type1, Shape::Type::type2, Shape::Type::type3}, \
     [](Calculator &c, Shape shape1, Shape shape2, Shape shape3, ...) { return c.command(*shape1.u.arg1, *shape2.u.arg2, *shape3.u.arg3); }}

#define UNARY_COMMAND(command, ...) \
    {#command, UNARY_COMMAND_OBJ(command, __VA_ARGS__)}
#define BINARY_COMMAND(command, ...) \
    {#command, BINARY_COMMAND_OBJ(command, __VA_ARGS__)}
#define TERNARY_COMMAND(command, ...) \
    {#command, TERNARY_COMMAND_OBJ(command, __VA_ARGS__)}

#define UNARY_OPERATOR(name) \
    {#name, {{Shape::Type::Number}, \
             [](Calculator &c, Shape shape, ...) { return c.get_##name(shape.u.n); }}}
#define BINARY_OPERATOR(name) \
    {#name, {{Shape::Type::Number, Shape::Type::Number}, \
             [](Calculator &c, Shape shape1, Shape shape2, ...) { return c.get_##name(shape1.u.n, shape2.u.n); }}}

const multimap<string,Command> commands = {
    // unary commands
    //  constructor
    UNARY_COMMAND(bisect, Segment, s),
    UNARY_COMMAND(bisect, Angle, a),
    UNARY_COMMAND(midpoint, Segment, s),
    //  scope
    UNARY_OPERATOR(sqrt),

    // binary commands
    //  scope
    {"straightedge", BINARY_COMMAND_OBJ(join_line, Point, p, Point, p)},
    {"compass", BINARY_COMMAND_OBJ(join_circle, Point, p, Point, p)},
    BINARY_COMMAND(meet, Circle, c, Circle, c),
    BINARY_COMMAND(meet, Line, l, Circle, c),
    BINARY_COMMAND(meet, Line, l, Line, l),
    //  constructor
    {"make_segment", BINARY_COMMAND_OBJ(join_segment, Point, p, Point, p)},
    BINARY_COMMAND(perpendicular, Line, l, Point, p),
    BINARY_COMMAND(parallel, Line, l, Point, p),
    BINARY_COMMAND(translate, Segment, s, Point, p),
    BINARY_COMMAND(translate, Angle, a, Point, p),
    BINARY_COMMAND(bisect, Point, p, Point, p),
    BINARY_COMMAND(midpoint, Point, p, Point, p),
    BINARY_COMMAND(reflect, Point, p, Point, p),
    BINARY_COMMAND(reflect, Point, p, Line, l),
    BINARY_COMMAND(reflect, Line, l, Point, p),
    BINARY_COMMAND(reflect, Line, l, Line, l),
    // calculator
    BINARY_OPERATOR(add),
    BINARY_OPERATOR(sub),
    BINARY_OPERATOR(mul),
    BINARY_OPERATOR(div),

    // ternary commands
    {"make_angle", TERNARY_COMMAND_OBJ(join_angle, Point, p, Point, p, Point, p)}
};

#endif // CLI_H
