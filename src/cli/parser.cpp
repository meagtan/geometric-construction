#include "include/cli.h"

#define PRED(n)  ((n) > 0 ? (n) - 1 : (n))
#define MIN(m,n) ((m) < (n) ? (m) : (n))

// CLIProgram

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
        cout << "    ";
        for (auto arg = pair->second.args.begin(); arg != pair->second.args.end(); ++arg) {
            cout << types[*arg];
            if (arg != pair->second.args.end() - 1)
                cout << "; ";
        }
        cout << endl;
    }
}

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
        return true;
    } else if (type == Shape::Number && parse_num(&num, input, pos, end)) {
        // if type is Number, try to parse num
        *shape = Shape(num);
        d.add(*shape);
        /*
        auto p = c.construct_number(num);
        if (p != nullptr) d.add(p);
        p = c.construct_number(num, 1);
        if (p != nullptr) d.add(p);
        */
        return true;
    } else if (type == Shape::Point) {
        // if type is Point, try to parse (num, num)

        // first search for ( , )
        if (input[pos] != '(' || input[end - 1] != ')')
            return false;
        for (comm = pos; comm < end && input[comm] != ','; ++comm);
        if (input[comm] != ',')
            return false;

        // parse arguments
        if (!parse_num(&num, input, pos+1, comm) || !parse_num(&num1, input, comm+1, end-1))
            return false;

        // check if point is constructed
        auto p = c.get_point(num, num1);
        if (p != nullptr) {
            *shape = Shape(p);
            d.add(*shape);
            return true;
        }
    }

    return false;
}

// parse input as a constructible number, if positive set the result to num, starting from index, skipping any leading or ending whitespace
// TODO distinguish - as a unary operator from - as a binary operator
bool CLIProgram::parse_num(constr_num *num, string str, int pos, int end)
{
    const string bin_ops = "+-*/";
    stack<constr_num> output;
    stack<char> operators;
    int n, p;
    char c, op;

    // read until empty
    while (pos < end) {
        // skip whitespace
        for (; pos < end && isspace(str[pos]); ++pos);
        if (pos == end)
            break;

        // if reading number, push number to output
        if (isdigit(str[pos])) {
            for (n = 0; pos < end && isdigit(str[pos]); n = n * 10 + str[pos] - '0', ++pos);
            output.push(n);
            continue;
        }

        // look for "sqrt", if found set pos at 't'
        // first check for an invalid lone 't'
        if (str[pos] == 't')
            return false;
        for (p = pos; p < MIN(end, pos + 3) && str[p] == "sqrt"[p-pos]; ++p);
        if (p < end && p == pos + 3 && str[p] == 't')
            pos = p;

        switch (c = str[pos++]) {
        case 't': // sqrt
        case '(':
            // if reading unary operator or left parenthesis, push to operators
            operators.push(c);
            break;
        case '+':
        case '-':
        case '*':
        case '/':
            // if reading binary operator, apply each binary operator in operators to output until the topmost operator has less precedence,
            //  or until the topmost operator is unary, and then push the operator to the stack
            while (!operators.empty() && bin_ops.find(op = operators.top()) >= bin_ops.find(c) && op != '(') {
                // apply op to output
                if (!apply(op, output))
                    return false;
                operators.pop();
            }
            operators.push(c);
            break;
        case ')':
            // if reading right parenthesis, pop and apply operator in operators until there is a left parenthesis,
            //  return if the stack is emptied before a left parenthesis, pop left parenthesis,
            //  if topmost operator is unary pop and apply it to output
            while (!operators.empty() && (op = operators.top()) != '(') {
                // apply op to output
                if (!apply(op, output))
                    return false;
                operators.pop();
            }
            if (operators.empty())
                return false;
            operators.pop();

            if (!operators.empty() && operators.top() == 't') {
                // apply operators.top() to output
                apply(operators.top(), output);
                operators.pop();
            }
            break;
        default: // wrong character
            return false;
        }
    }

    while (!operators.empty()) {
        // pop operator from operators
        c = operators.top();
        operators.pop();

        // if left parenthesis, invalid expression
        if (c == '(') return false;

        // else apply operator to output
        apply(c, output);
    }

    if (!output.empty())
        *num = output.top();
    return true;
}

#define OP_CASE(op, ch) case ch: arg1 = output.top(); output.pop(); if (output.empty()) return false; output.top() = output.top() op arg1; break;

bool CLIProgram::apply(int op, stack<constr_num> &output)
{
    constr_num arg1;

    if (output.empty())
        return false;

    switch (op) {
    case 't':
        output.top() = sqrt(output.top());
        break;
    OP_CASE(+, '+');
    OP_CASE(-, '-');
    OP_CASE(*, '*');
    OP_CASE(/, '/');
    }

    return true;
}

#undef OP_CASE
