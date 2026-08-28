#include <iostream>
#include <windows.h>
#include "token.cpp"
#include "lexer.cpp"
using namespace std;

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    Lexer lex("integer x : 5 + 3");
    string error;
    lex.tokenize(error);

    if (error != "") {
        cout << error << endl;
    } else {
        for (int i = 0; i < (int)lex.tokens.size(); i++) {
            cout << lex.tokens[i].toString() << endl;
        }
    }
    return 0;
}
