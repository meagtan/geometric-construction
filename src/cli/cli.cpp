#include "include/cli.h"

#define PRED(n) ((n) > 0 ? (n) - 1 : (n))

// CLIProgram

CLIProgram::CLIProgram() : c(this)
{
    cout << "Type help to see available commands.\n";
}

CLIProgram::~CLIProgram() {}

// TODO add assignment
void CLIProgram::input(string query)
{
    size_t start, comm_end;
    vector<Shape> args;
    Shape arg;
    bool found;

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
        auto comm = pair->second;
        args.clear();
        found = true;

        // skip for invalid number of arguments
        if (std::count(query.begin() + comm_end, query.end(), delim) != PRED(comm.args.size()))
            continue;

        // try to parse each argument based on its type
        for (int i = 0, pos = comm_end, del = 0; i < comm.args.size(); ++i) {
            // find next delimiter or end of line
            for (del = 0; pos + del != query.length() && query[pos + del] != delim; ++del);

            cout << "\"" << query << "\"[" << pos << ":" << pos + del << "] vs. " << comm.args[i] << endl;
            // if argument cannot be parsed, skip to next command
            if (!parse_arg(&arg, query.substr(pos, del), comm.args[i])) {
                found = false;
                break;
            }

            args.push_back(arg);

            // move to after next delim and skip whitespace
            pos += del + 1;
            for (; pos != query.length() && isspace(query[pos]); ++pos);
        }

        // apply args to command and exit
        if (found) {
            (*comm.fun)(*this, c, args.data());
            return;
        }
    }

    // no matching command found, alert user
    cout << "Error: invalid set of arguments for command " << comm_range.first->first << "." << endl;
    cout << "The valid lists of arguments for " << comm_range.first->first << " are:" << endl;
    for (auto pair = comm_range.first; pair != comm_range.second; ++pair) {
        cout << '\t';
        for (auto arg = pair->second.args.begin(); arg != pair->second.args.end(); ++arg) {
            cout << types[*arg];
            if (arg != pair->second.args.end() - 1)
                cout << "; ";
        }
        cout << endl;
    }
}

void CLIProgram::quit() { running = false; }
bool CLIProgram::is_running() { return running; }

void CLIProgram::help()
{
    printf("  The list of available commands are:\n\t%-15sArguments\n", "Command");
    for (auto &pair : commands) {
        printf("\t%-15s", pair.first.c_str());
        for (auto type : pair.second.args)
            printf("%-10s", types[type].c_str());
        cout << endl;
    }
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

Shape::Shape(const Shape &other) : type(other.type), u(other.u), name(other.name)
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
        name = other.name;
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

Dictionary::Dictionary()
{
    add("origin", origin);
    add("x_axis", x_axis);
    add("y_axis", y_axis);
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
