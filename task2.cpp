// Task 2: Expression Validators Using Stack
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <cctype>
#include "stack.h"
using namespace std;

// Part A
bool isValidBrackets(string s) {
    ArrayStack<char> st;
    for (size_t i = 0; i < s.size(); i++) {
        char c = s[i];
        if (c == '(' || c == '{' || c == '[') {
            st.push(c);
        } else if (c == ')' || c == '}' || c == ']') {
            if (st.empty()) return false;                    // nothing to close
            char open = st.pop();
            if ((c == ')' && open != '(') ||
                (c == '}' && open != '{') ||
                (c == ']' && open != '[')) return false;     // wrong type
        }
    }
    return st.empty();                                       // leftovers = unclosed
}

// Part B
struct OpenTag {
    string name;
    int    pos;     
};

static bool isVoidTag(const string& n) {
    const char* v[] = { "br", "hr", "img", "input", "meta", "link" };
    for (const char* x : v) if (n == x) return true;
    return false;
}

static bool isNameChar(char c) {
    return isalnum((unsigned char)c) || c == '-' || c == '_' || c == ':';
}

// Returns (is_valid, list_of_errors). Names are case-sensitive .
// Handles: nesting, self-closing <br/>, void tags 
// attributes (even containing '>' inside quotes), comments, <!DOCTYPE>, <?xml?>.
pair<bool, vector<string>> validate_markup(string s) {
    vector<string> errors;
    ArrayStack<OpenTag> st;
    size_t i = 0, n = s.size();

    while (i < n) {
        if (s[i] != '<') { i++; continue; }                  // plain text
        int pos = (int)i;

        // comment <!-- ... -->
        if (s.compare(i, 4, "<!--") == 0) {
            size_t e = s.find("-->", i + 4);
            if (e == string::npos) { errors.push_back("Unterminated comment at position " + to_string(pos)); break; }
            i = e + 3;
            continue;
        }

        // find the closing '>' that is not inside quotes
        size_t e = i + 1;
        char quote = 0;
        while (e < n && (quote || s[e] != '>')) {
            if (quote) { if (s[e] == quote) quote = 0; }
            else if (s[e] == '"' || s[e] == '\'') quote = s[e];
            e++;
        }
        if (e >= n) { errors.push_back("Malformed tag at position " + to_string(pos) + ": missing '>'"); break; }

        string inner = s.substr(i + 1, e - i - 1);
        i = e + 1;

        if (inner.empty()) { errors.push_back("Empty tag '<>' at position " + to_string(pos)); continue; }
        if (inner[0] == '!' || inner[0] == '?') continue;    // DOCTYPE / xml declaration

        bool closing   = (inner[0] == '/');
        bool selfClose = (inner.back() == '/');
        size_t k = closing ? 1 : 0;
        string name;
        while (k < inner.size() && isNameChar(inner[k])) name += inner[k++];

        if (name.empty()) { errors.push_back("Invalid tag name at position " + to_string(pos)); continue; }

        if (closing) {
            if (st.empty()) {
                errors.push_back("Unexpected closing tag </" + name + "> at position " + to_string(pos) + " (no open tag)");
                continue;
            }
            // is this name open anywhere in the stack?
            int found = -1;
            for (int d = st.size() - 1; d >= 0; d--)
                if (st.peekAt(d).name == name) { found = d; break; }

            if (found == -1) {
                errors.push_back("Unexpected closing tag </" + name + "> at position " + to_string(pos) +
                                 " (expected </" + st.top().name + ">)");
            } else {
                while (st.size() - 1 > found) {              // tags left open inside it
                    OpenTag t = st.pop();
                    errors.push_back("Tag <" + t.name + "> opened at position " + to_string(t.pos) +
                                     " was not closed before </" + name + ">");
                }
                st.pop();                                    // the matching tag
            }
        } else if (!selfClose && !isVoidTag(name)) {
            st.push(OpenTag{ name, pos });
        }
    }

    while (!st.empty()) {
        OpenTag t = st.pop();
        errors.push_back("Unclosed tag <" + t.name + "> opened at position " + to_string(t.pos));
    }
    return make_pair(errors.empty(), errors);
}

int main() {
    cout << "=== Part A: Bracket Validator ===\n";
    const char* b[] = { "{[()]}", "([)]", "((", "))", "", "a(b)[c]{d}", "{[(])}", "()[]{}", "(()" };
    for (const char* t : b)
        cout << "\"" << t << "\"  ->  " << (isValidBrackets(t) ? "VALID" : "INVALID") << "\n";

    cout << "\n=== Part B: HTML/XML Tag Validator ===\n";
    const char* m[] = {
        "<html><body><p>Hello</p></body></html>",
        "<div><p>text</div></p>",
        "<div><br/><img src=\"a.png\"></div>",
        "<ul><li>One<li>Two</ul>",
        "</div>",
        "<div><span></div>",
        "<a href=\"x>y\">link</a>",
        "<!DOCTYPE html><!-- note --><html><head></head></html>",
        "<b><i>bold italic</b></i>",
        "<p>unfinished"
    };
    for (const char* t : m) {
        pair<bool, vector<string>> r = validate_markup(t);
        cout << t << "\n  -> " << (r.first ? "VALID" : "INVALID") << "\n";
        for (const string& err : r.second) cout << "     * " << err << "\n";
    }
    return 0;
}