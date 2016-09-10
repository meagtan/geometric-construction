#ifndef CLI_H
#define CLI_H

#include "calculator.h"

#include <algorithm>
#include <cctype>
#include <cstdarg>
#include <functional>
#include <map>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>

using std::cout;
using std::endl;
using std::function;
using std::isspace;
using std::istringstream;
using std::map;
using std::pair;
using std::stack;
using std::string;
using std::unordered_map;
using std::unordered_multimap;

// all this for what would amount to a simple apply call in lisp

struct Shape {
    enum Type {
        Point,
        Line,
        Circle,
        Segment, // TODO implement bit basis for Type to make inheritance work
        Angle,
        Number
    } type;
    union Member {
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
    constr_num n;

    Shape();
    Shape(const struct Point  *);
    Shape(const struct Line   *);
    Shape(const struct Circle *);
    Shape(const LineSegment   *);
    Shape(const struct Angle  *);
    Shape(constr_num);
    Shape(const Shape &);
    ~Shape(); // does not delete pointer

    Shape &operator=(const Shape &);
    bool operator==(const Shape &) const;
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

    bool running = true;

    void straightedge(const Point*, const Point*, const Line*);
    void compass(const Point*, const Point*, const Circle*);
    void meet(const Circle*, const Circle*, const Point*);
    void meet(const Line*, const Circle*, const Point*);
    void meet(const Line*, const Line*, const Point*);

public: // TODO remove this after testing
    bool parse_arg(Shape *shape, string input, Shape::Type type);
    bool parse_num(constr_num *num, string input, int pos, int end);
    bool apply(int op, stack<constr_num> &output);
public:
    CLIProgram();
    ~CLIProgram();

    void input(string query);
    void add(Shape);
    void quit();
    void help();
    bool is_running();

    void print(const Point &);
    void print(const Line &);
    void print(const Circle &);
    void print(const LineSegment &);
    void print(const Angle &);
    void print(constr_num);
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

#define PRINT_COMM(_type, _arg) \
    {"print", Command({Shape::Type::_type}, +[](CLIProgram &p, Calculator &, Shape shapes[]) { \
                                                   p.print(*shapes[0].u._arg); })}

const unordered_multimap<string,Command> commands ({
    {"help", Command({}, +[](CLIProgram &p, Calculator &, Shape[]) {p.help();})},
    {"quit", Command({}, +[](CLIProgram &p, Calculator &, Shape[]) {p.quit();})},
    PRINT_COMM(Point, p),
    PRINT_COMM(Line, l),
    PRINT_COMM(Circle, c),
    PRINT_COMM(Segment, s),
    PRINT_COMM(Angle, a),
    {"print", Command({Shape::Type::Number}, +[](CLIProgram &p, Calculator &, Shape shapes[]) {p.print(shapes[0].n);})},
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

#undef UNARY_COMMAND
#undef UNARY_COMMAND_OBJ
#undef UNARY_OPERATOR
#undef BINARY_COMMAND
#undef BINARY_COMMAND_OBJ
#undef BINARY_OPERATOR
#undef TERNARY_COMMAND_OBJ
#undef PRINT_COMM

constexpr char delim = ';'; // delimiter for arguments
const string types[] = {"Point", "Line", "Circle", "Segment", "Angle", "Number"};

#endif // CLI_H
