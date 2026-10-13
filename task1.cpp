// Task 1: Expression Conversion and Evaluation
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <cctype>
#include <iomanip>
#include <stdexcept>
#include "stack.h"
using namespace std;

// helpers
static int prec(char op) {
    switch (op) {
        case '^': return 3;
        case '*': case '/': return 2;
        case '+': case '-': return 1;
        default:  return 0;
    }
}
static bool isOpChar(char c)  { return c == '+' || c == '-' || c == '*' || c == '/' || c == '^'; }
static bool isOperand(const string& t) { return !t.empty() && (isalnum((unsigned char)t[0]) || t[0] == '.'); }
static bool isOperator(const string& t) { return t.size() == 1 && isOpChar(t[0]); }

// Split an infix string into tokens: operands, operators, parentheses.
static vector<string> tokenize(const string& s) {
    vector<string> tk;
    size_t i = 0;
    while (i < s.size()) {
        char c = s[i];
        if (isspace((unsigned char)c)) { i++; continue; }
        if (isalnum((unsigned char)c) || c == '.') {
            string num;
            while (i < s.size() && (isalnum((unsigned char)s[i]) || s[i] == '.')) num += s[i++];
            tk.push_back(num);
        } else if (isOpChar(c) || c == '(' || c == ')') {
            tk.push_back(string(1, c));
            i++;
        } else {
            throw runtime_error(string("Invalid character '") + c + "' in expression");
        }
    }
    return tk;
}

// Split a postfix/prefix string on whitespace.
static vector<string> splitWords(const string& s) {
    vector<string> w;
    istringstream in(s);
    string t;
    while (in >> t) w.push_back(t);
    return w;
}

static string join(const vector<string>& v) {
    string r;
    for (size_t i = 0; i < v.size(); i++) { if (i) r += ' '; r += v[i]; }
    return r;
}

// Shunting-yard. When reversedForPrefix is true the associativity rule is
// flipped (the input is a reversed infix expression, see infix_to_prefix).
static vector<string> shuntingYard(const vector<string>& tk, bool reversedForPrefix) {
    ArrayStack<char> ops;
    vector<string> out;
    for (size_t i = 0; i < tk.size(); i++) {
        const string& t = tk[i];
        if (isOperand(t)) {
            out.push_back(t);
        } else if (t == "(") {
            ops.push('(');
        } else if (t == ")") {
            while (!ops.empty() && ops.top() != '(') out.push_back(string(1, ops.pop()));
            if (ops.empty()) throw runtime_error("Mismatched parentheses");
            ops.pop();                                   // discard '('
        } else {                                         // operator
            char cur = t[0];
            bool rightAssoc = (cur == '^');
            if (reversedForPrefix) rightAssoc = !rightAssoc;
            while (!ops.empty() && ops.top() != '(') {
                char top = ops.top();
                bool popIt = prec(top) > prec(cur) || (prec(top) == prec(cur) && !rightAssoc);
                if (!popIt) break;
                out.push_back(string(1, ops.pop()));
            }
            ops.push(cur);
        }
    }
    while (!ops.empty()) {
        char c = ops.pop();
        if (c == '(') throw runtime_error("Mismatched parentheses");
        out.push_back(string(1, c));
    }
    return out;
}

static double toNumber(const string& t) {
    size_t used = 0;
    double v;
    try { v = stod(t, &used); }
    catch (...) { throw runtime_error("Operand '" + t + "' is not a number"); }
    if (used != t.size()) throw runtime_error("Operand '" + t + "' is not a number");
    return v;
}

static double applyOp(char op, double a, double b) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': if (b == 0) throw runtime_error("Division by zero"); return a / b;
        case '^': return pow(a, b);
    }
    throw runtime_error("Unknown operator");
}

// ------------------------------------------------------- required functions
// 1. Infix -> Postfix
string infix_to_postfix(string expr) {
    return join(shuntingYard(tokenize(expr), false));
}

// 2. Infix -> Prefix
string infix_to_prefix(string expr) {
    vector<string> tk = tokenize(expr);
    vector<string> rev(tk.rbegin(), tk.rend());
    for (size_t i = 0; i < rev.size(); i++) {
        if (rev[i] == "(") rev[i] = ")";
        else if (rev[i] == ")") rev[i] = "(";
    }
    vector<string> post = shuntingYard(rev, true);
    vector<string> pre(post.rbegin(), post.rend());
    return join(pre);
}

// 3. Postfix -> Infix
string postfix_to_infix(string expr) {
    vector<string> w = splitWords(expr);
    ArrayStack<string> st;
    for (size_t i = 0; i < w.size(); i++) {
        if (isOperand(w[i])) {
            st.push(w[i]);
        } else if (isOperator(w[i])) {
            if (st.size() < 2) throw runtime_error("Invalid postfix expression (missing operand)");
            string b = st.pop();
            string a = st.pop();
            st.push("(" + a + " " + w[i] + " " + b + ")");
        } else {
            throw runtime_error("Invalid token '" + w[i] + "'");
        }
    }
    if (st.size() != 1) throw runtime_error("Invalid postfix expression");
    string r = st.pop();
    if (r.size() > 2 && r.front() == '(') r = r.substr(1, r.size() - 2);
    return r;
}

// 4. Evaluate postfix
double evaluate_postfix(string expr) {
    vector<string> w = splitWords(expr);
    ArrayStack<double> st;
    for (size_t i = 0; i < w.size(); i++) {
        if (isOperand(w[i])) {
            st.push(toNumber(w[i]));
        } else if (isOperator(w[i])) {
            if (st.size() < 2) throw runtime_error("Invalid postfix expression (missing operand)");
            double b = st.pop();
            double a = st.pop();
            st.push(applyOp(w[i][0], a, b));
        } else {
            throw runtime_error("Invalid token '" + w[i] + "'");
        }
    }
    if (st.size() != 1) throw runtime_error("Invalid postfix expression");
    return st.pop();
}

// 5. Evaluate prefix
double evaluate_prefix(string expr) {
    vector<string> w = splitWords(expr);
    ArrayStack<double> st;
    for (int i = (int)w.size() - 1; i >= 0; i--) {
        if (isOperand(w[i])) {
            st.push(toNumber(w[i]));
        } else if (isOperator(w[i])) {
            if (st.size() < 2) throw runtime_error("Invalid prefix expression (missing operand)");
            double a = st.pop();
            double b = st.pop();
            st.push(applyOp(w[i][0], a, b));
        } else {
            throw runtime_error("Invalid token '" + w[i] + "'");
        }
    }
    if (st.size() != 1) throw runtime_error("Invalid prefix expression");
    return st.pop();
}

int main() {
    cout << setprecision(10);

    cout << "=== Symbolic expressions (conversion only) ===\n";
    const char* symbolic[] = {
        "A+B*C", "(A+B)*C-D/E", "A^B^C", "A-B-C", "((A+B)*(C-D))/E"
    };
    for (const char* e : symbolic) {
        string post = infix_to_postfix(e);
        string pre  = infix_to_prefix(e);
        cout << "Infix   : " << e << "\n"
             << "Postfix : " << post << "\n"
             << "Prefix  : " << pre << "\n"
             << "Postfix -> Infix : " << postfix_to_infix(post) << "\n\n";
    }

    cout << "=== Numeric expressions (conversion + evaluation) ===\n";
    const char* numeric[] = {
        "(12+8)*3-6/2", "3+4*2/(1-5)^2^3", "2^3^2", "100/10/5", "7-2-1"
    };
    for (const char* e : numeric) {
        string post = infix_to_postfix(e);
        string pre  = infix_to_prefix(e);
        cout << "Infix   : " << e << "\n"
             << "Postfix : " << post << "   = " << evaluate_postfix(post) << "\n"
             << "Prefix  : " << pre  << "   = " << evaluate_prefix(pre) << "\n\n";
    }

    cout << "=== Error handling ===\n";
    try { infix_to_postfix("(A+B"); } catch (exception& e) { cout << "\"(A+B\"    -> " << e.what() << "\n"; }
    try { infix_to_postfix("A+B)"); } catch (exception& e) { cout << "\"A+B)\"    -> " << e.what() << "\n"; }
    try { evaluate_postfix("5 5 + +"); } catch (exception& e) { cout << "\"5 5 + +\" -> " << e.what() << "\n"; }
    try { evaluate_postfix("8 0 /"); } catch (exception& e) { cout << "\"8 0 /\"    -> " << e.what() << "\n"; }
    return 0;
}