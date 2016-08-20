#ifndef CLI_H
#define CLI_H

#include "calculator.h"
#include <string>
#include <unordered_map>
#include <tuple>

using std::string;
using std::unordered_map;
using std::tuple;

class CLIProgram {
    struct CLIListener : MoveListener {
        void straightedge(const Point*, const Point*, const Line*);
        void compass(const Point*, const Point*, const Circle*);
        void meet(const Circle*, const Circle*, const Point*);
        void meet(const Line*, const Circle*, const Point*);
        void meet(const Line*, const Line*, const Point*);
    } &lis;
    Calculator &c;
    Dictionary &d;

public:
    CLIProgram();
    ~CLIProgram();

    void input(string query);
};

struct Shape {
    const enum Type {
        Point,
        Line,
        Circle,
        Angle,
        Number
    } type;
    const union {
        const Point  *p;
        const Line   *l;
        const Circle *c;
        const Angle  *a;
        constr_num n;
    } u;

    Shape(const Point  *);
    Shape(const Line   *);
    Shape(const Circle *);
    Shape(const Angle  *);
    Shape(constr_num);
    ~Shape(); // does not delete pointer
};

struct Dictionary {
    unordered_map<string,Shape> shapes;

    Dictionary();
    ~Dictionary();

    Shape get_shape(string name);
    string get_name(Shape name);

    // automatically coerces pointers taken as arguments
    string add(Shape shape); // assigns default name to shape
    string add(string name, Shape shape); // assigns given name to shape if not already assigned, in which case assigns default name
};

struct Command {
    // pointer to function taking shapes and returning a shape
    // types of arguments
};

#define MAKE_COMMAND(command, ...) {#command, Command(&command)}

const unordered_map<string,Command> commands {
    MAKE_COMMAND(straightedge, Point, Point),
    MAKE_COMMAND(compass, Point, Point),
    MAKE_COMMAND(meet, Circle, Circle),
    MAKE_COMMAND(meet, Line, Circle),
    MAKE_COMMAND(meet, Line, Line)
    // TODO add more
};

#endif // CLI_H
