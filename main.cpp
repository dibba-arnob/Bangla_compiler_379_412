#include <iostream>
#include <windows.h>
#include "token.cpp"
#include "lexer.cpp"
#include "node.cpp"
#include "parser.cpp"
using namespace std;

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    Lexer l("a+b*c+(d-10)");
    string e;
    l.tokenize(e);

    if (e != "") {
        cout << e << endl;
        return 1;
    }

    Parser p(l.tokens);
    Node* ast = p.parse();

    if (p.error != "") {
        cout << p.error << endl;
    } else {
        cout << ast->toString() << endl;
    }

    return 0;
}
