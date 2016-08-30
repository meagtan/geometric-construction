#include "include/cli.h"

using std::cout;
using std::endl;

// CLIProgram

void CLIProgram::parse_arg(Shape *shape, string input, Shape::Type type)
{
    Shape *res;
    constr_num *num = nullptr, *num1 = nullptr;
    istringstream stream(input);
    string name;

    stream >> name;
    res = d.get_shape(name);
    stream.str(input);
    if (res != nullptr && res->type == type) {
        *shape = *res;
    } else if (type == Shape::Number) {
        // if type is Number, try to parse num
        parse_num(num, stream);
        if (num != nullptr && !stream.rdbuf()->in_avail())
            *shape = Shape(*num);
    } else if (type == Shape::Point) {
        // if type is Point, try to parse (num, num)

        if (stream.get() != '(') return;

        // parse first argument
        parse_num(num, stream);
        if (num == nullptr || stream.get() != ',') return;

        // parse second argument
        parse_num(num1, stream);
        if (num1 == nullptr || stream.get() != ')') return;

        // check if point is constructed
        auto p = c.get_point(*num, *num1);
        if (p != nullptr)
            *shape = Shape(p);

    }
}

// parse input as a constructible number, if positive set the result to num, starting from index, skipping any leading or ending whitespace
// TODO distinguish - as a unary operator from - as a binary operator
void CLIProgram::parse_num(constr_num *num, std::istream &str)
{
    const string bin_ops = "+-*/";
    stack<constr_num> output;
    stack<char> operators;
    int n;

    // read until empty
    while (str.rdbuf()->in_avail()) {
        // if reading number, push number to output
        str >> n;
        if (str.good()) {
            output.push(n);
            continue;
        }
        str.clear();

        switch (str.peek()) {
        case 's': // for now, take sqrt to be written as 's'
        case ')':
            // if reading unary operator or right parenthesis, push to operators
            operators.push(str.get());
            break;
        case '+':
        case '-':
        case '*':
        case '/':
            // if reading binary operator, apply each binary operator in operators to output until the topmost operator has less precedence,
            //  or until the topmost operator is unary, and then push the operator to the stack
            for (char op = operators.top(); !operators.empty() && bin_ops.find(op) >= bin_ops.find(str.peek()); op = operators.top()) {
                // apply op to output
                apply(op, output);
                operators.pop();
            }
            operators.push(str.get());
            break;
        case '(':
            // if reading left parenthesis, pop and apply operator in operators until there is a right parenthesis,
            //  return if the stack is emptied before a right parenthesis, pop right parenthesis,
            //  if topmost operator is unary pop and apply it to output
            for (char op = operators.top(); !operators.empty() && op != ')'; op = operators.top()) {
                // apply op to output
                apply(op, output);
                operators.pop();
            }
            if (operators.empty())
                return;
            operators.pop();
            if (!operators.empty() && operators.top() == 's') {
                // apply operators.top() to output
                apply(operators.top(), output);
                operators.pop();
            }
            str.get();
        default:
            str.get();
        }

        // skip whitespace
        str >> std::ws;
    }

    while (!operators.empty()) {
        // pop operator from operators
        // if right parenthesis, return
        // else apply operator to output
    }

    if (!output.empty())
        *num = output.top();
}

#define OP_CASE(op, ch) case ch: arg1 = output.top(); output.pop(); output.top() = output.top() op arg1; break;

void CLIProgram::apply(int op, stack<constr_num> &output)
{
    constr_num arg1;

    switch (op) {
    case 's':
        output.top() = sqrt(output.top());
        break;
    OP_CASE(+, '+');
    OP_CASE(-, '-');
    OP_CASE(*, '*');
    OP_CASE(/, '/');
    }
}

#undef OP_CASE

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
}

void CLIProgram::print(const Point &p)
{
    cout << "The point (" << p.x << "," << p.y << ")" << endl;
}

void CLIProgram::print(const Line &l)
{
    cout << "The line " << l.x_coeff << " x + " << l.y_coeff << " y + " << l.const_coeff << " = 0" << endl;
}

void CLIProgram::print(const Circle &c)
{
    cout << "The circle with center (" << c.center.x << "," << c.center.y << ") and radius " << c.radius << endl;
}

void CLIProgram::print(const LineSegment &s)
{
    cout << "The line segment between points (" << s.start.x << "," << s.start.y << ") and (" << s.end.x << "," << s.end.y << ")" << endl;
}

void CLIProgram::print(const Angle &a)
{
    cout << "The angle between lines \n\t" <<
            a.l1.x_coeff << " x + " << a.l1.y_coeff << " y + " << a.l1.const_coeff << " = 0 and \n\t" <<
            a.l1.x_coeff << " x + " << a.l1.y_coeff << " y + " << a.l1.const_coeff << " = 0" << endl;

}

void CLIProgram::print(constr_num n)
{
    cout << "The constructible number " << n << endl;
}
