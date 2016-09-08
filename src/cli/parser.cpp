#include "include/cli.h"

using std::cout;
using std::endl;

// CLIProgram

bool CLIProgram::parse_arg(Shape *shape, string input, Shape::Type type)
{
    Shape *res;
    constr_num num, num1;
    istringstream stream(input);
    string name;
    int pos = 0, comm, end = input.length();

    stream >> name;
    res = d.get_shape(name);
    if (res != nullptr && res->type == type) {
        *shape = *res;
    } else if (type == Shape::Number) {
        // if type is Number, try to parse num
        if (parse_num(&num, input, pos, end))
            *shape = Shape(num);
        else
            return false;
    } else if (type == Shape::Point) {
        // if type is Point, try to parse (num, num)

        // first search for ( , )
        if (input[pos] != '(' || input[end] != ')')
            return false;
        for (comm = pos; comm < end && input[comm] != ')' && input[comm] != ','; ++comm);
        if (input[comm] != ',')
            return false;

        // parse arguments
        if (!parse_num(&num, input, pos, comm) || !parse_num(&num1, input, comm + 1, end))
            return false;

        // check if point is constructed
        auto p = c.get_point(num, num1);
        if (p != nullptr)
            *shape = Shape(p);
        else
            return false;
    }

    return true;
}

// parse input as a constructible number, if positive set the result to num, starting from index, skipping any leading or ending whitespace
// TODO distinguish - as a unary operator from - as a binary operator
bool CLIProgram::parse_num(constr_num *num, string str, int pos, int end)
{
    const string bin_ops = "+-*/";
    stack<constr_num> output;
    stack<char> operators;
    int n;
    char c;

    // read until empty
    while (pos < end) {
        // if reading number, push number to output
        if (isdigit(str[pos])) {
            for (n = 0; pos < end && isdigit(str[pos]); n = n * 10 + str[pos] - '0', ++pos);
            output.push(n);
            continue;
        }

        switch (c = str[pos]) {
        case 's': // for now, take sqrt to be written as 's'
        case ')':
            // if reading unary operator or right parenthesis, push to operators
            operators.push(c);
            break;
        case '+':
        case '-':
        case '*':
        case '/':
            // if reading binary operator, apply each binary operator in operators to output until the topmost operator has less precedence,
            //  or until the topmost operator is unary, and then push the operator to the stack
            for (char op = operators.top(); !operators.empty() && bin_ops.find(op) >= bin_ops.find(c); op = operators.top()) {
                // apply op to output
                apply(op, output);
                operators.pop();
            }
            operators.push(c);
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
                return false;
            operators.pop();
            if (!operators.empty() && operators.top() == 's') {
                // apply operators.top() to output
                apply(operators.top(), output);
                operators.pop();
            }
        }


        // skip whitespace
        for (; pos < end && isspace(str[pos]); ++pos);
    }

    while (!operators.empty()) {
        // pop operator from operators
        c = operators.top();
        operators.pop();

        // if right parenthesis, invalid expression
        if (c == ')') return false;

        // else apply operator to output
        apply(c, output);
    }

    if (!output.empty())
        *num = output.top();
    return true;
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
