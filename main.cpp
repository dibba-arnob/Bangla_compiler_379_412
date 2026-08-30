#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>
#include "token.cpp"
#include "lexer.cpp"
#include "node.cpp"
#include "parser.cpp"
#include "visualizer.cpp"
using namespace std;

string readFile(string path, bool &ok) {
    ifstream file(path, ios::binary);
    if (!file.is_open()) {
        ok = false;
        cout << "Tried to open: " << path << endl;
        return "";
    }
    stringstream buffer;
    buffer << file.rdbuf();
    ok = true;
    return buffer.str();
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    bool fileOk = false;
    string code = readFile("D:\\LEADING U\\8th sem\\compiler\\New folder\\c++\\bangla_compiler_cpp\\bangla_compiler\\bangla compiler in cpp\\bin\\Debug\\code.txt", fileOk);

    if (!fileOk) {
        return 1;
    }

    Lexer l(code);
    string e;
    l.tokenize(e);

    if (e != "") {
        cout << e << endl;
        return 1;
    }

    cout << "----- TOKENS -----" << endl;
    for (int i = 0; i < (int)l.tokens.size(); i++) {
        cout << l.tokens[i].toString() << endl;
    }
    cout << "------------------" << endl;

    Parser p(l.tokens);
    Node* ast = p.parse();

    if (p.error != "") {
        cout << p.error << endl;
        return 1;
    }

    cout << "----- PARSE TREE (string) -----" << endl;
    cout << ast->toString() << endl;

    cout << "----- PARSE TREE (diagram) -----" << endl;
    tree(ast);

    return 0;
}
