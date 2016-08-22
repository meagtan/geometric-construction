#ifndef CLI_H
#define CLI_H

#include "calculator.h"
#include <string>
#include <map>
#include <unordered_map>
#include <utility>
#include <cstdarg>
#include <functional>

using std::string;
using std::map;
using std::pair;
using std::unordered_multimap;
using std::function;

// all this for what would amount to a simple apply call in lisp

struct Shape {
    const enum Type {
        Point,
        Line,
        Circle,
        Segment, // TODO implement bit basis for Type to make inheritance work
        Angle,
        Number
    } type;
    const union Member {
        const struct Point  *p;
        const struct Line   *l;
        const struct Circle *c;
        const LineSegment *s;
        const struct Angle  *a;

        Member(const struct Point  *);
        Member(const struct Line   *);
        Member(const struct Circle *);
        Member(const LineSegment   *);
        Member(const struct Angle  *);
    } u;
    const constr_num n;

    Shape(const struct Point  *);
    Shape(const struct Line   *);
    Shape(const struct Circle *);
    Shape(const LineSegment   *);
    Shape(const struct Angle  *);
    Shape(constr_num);
    Shape(const Shape &);
    ~Shape(); // does not delete pointer
private:
    string name = "";
    friend struct Dictionary;
};

struct Dictionary {
    map<string,Shape> shapes;

    Dictionary();
    ~Dictionary();

    Shape *get_shape(string name);
    string get_name(Shape shape);

    // automatically coerces pointers taken as arguments
    string add(Shape shape); // assigns default name to shape
    string add(string name, Shape shape); // assigns given name to shape if not already assigned, in which case assigns default name
private:
    int counter[6] = {0};
    char letters[6] = {'P', 'L', 'C', 'S', 'A', 'N'};
    string generate_name(int type);
};

class CLIProgram : protected MoveListener {
    friend class Calculator;

    Calculator c;
    Dictionary d;

    void straightedge(const Point*, const Point*, const Line*);
    void compass(const Point*, const Point*, const Circle*);
    void meet(const Circle*, const Circle*, const Point*);
    void meet(const Line*, const Circle*, const Point*);
    void meet(const Line*, const Line*, const Point*);

public:
    CLIProgram();
    ~CLIProgram();

    void input(string query);

    void add(Shape);
};

typedef void (*CommandFunc)(CLIProgram &, Calculator &, Shape[]);

struct Command {
    vector<Shape::Type> args;
    CommandFunc fun;

    Command(vector<Shape::Type> args, CommandFunc fun);
    ~Command() = default;
};

#define UNARY_COMMAND_OBJ(command, type, arg) \
    Command({Shape::Type::type}, \
        +[](CLIProgram &p, Calculator &c, Shape shapes[]) { p.add(c.command(*shapes[0].u.arg)); })
#define BINARY_COMMAND_OBJ(command, type1, arg1, type2, arg2) \
    Command({Shape::Type::type1, Shape::Type::type2}, \
        +[](CLIProgram &p, Calculator &c, Shape shapes[]) { p.add(c.command(*shapes[0].u.arg1, *shapes[1].u.arg2)); })
#define TERNARY_COMMAND_OBJ(command, type1, arg1, type2, arg2, type3, arg3) \
    Command({Shape::Type::type1, Shape::Type::type2, Shape::Type::type3},\
        +[](CLIProgram &p, Calculator &c, Shape shapes[]) { p.add(c.command(*shapes[0].u.arg1, *shapes[1].u.arg2, *shapes[2].u.arg3)); })

#define UNARY_COMMAND(command, ...) \
    {#command, UNARY_COMMAND_OBJ(command, __VA_ARGS__)}
#define BINARY_COMMAND(command, ...) \
    {#command, BINARY_COMMAND_OBJ(command, __VA_ARGS__)}
#define TERNARY_COMMAND(command, ...) \
    {#command, TERNARY_COMMAND_OBJ(command, __VA_ARGS__)}

#define UNARY_OPERATOR(name) \
    {#name, Command({Shape::Type::Number}, \
             +[](CLIProgram &p, Calculator &c, Shape shapes[]) { p.add(c.get_##name(shapes[0].n)); })}
#define BINARY_OPERATOR(name) \
    {#name, Command({Shape::Type::Number, Shape::Type::Number}, \
             +[](CLIProgram &p, Calculator &c, Shape shapes[]) { p.add(c.get_##name(shapes[0].n, shapes[1].n)); })}

#define BINARY_COMMAND_PAIR(command, type1, arg1, type2, arg2) \
    {#command, Command({Shape::Type::type1, Shape::Type::type2}, \
                        +[](CLIProgram &p, Calculator &c, Shape shapes[]) { \
                            auto pair = c.command(*shapes[0].u.arg1, *shapes[1].u.arg2); \
                            p.add(pair.first); p.add(pair.second);})}

const unordered_multimap<string,Command> commands ({
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
    BINARY_COMMAND_PAIR(meet, Circle, c, Circle, c), // doesn't work for pairs, instead send result to CLIProgram
    BINARY_COMMAND_PAIR(meet, Line, l, Circle, c),
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
});

#endif // CLI_H
