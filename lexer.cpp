#ifndef LEXER_CPP
#define LEXER_CPP

#include <string>
#include <vector>
#include "token.cpp"
using namespace std;

// ... rest of Lexer class unchanged ...


// lexer:
class Lexer {
public:
    string code;
    int pos;
    int line;
    vector<Token> tokens;

    Lexer(string code) {
        this->code = code;
        pos = 0;
        line = 0;
    }

    char get_char() {
        if (pos < (int)code.length()) {
            return code[pos];
        } else {
            return '\0';   // '\0' মানে None (kono char nai)
        }
    }

    void advance() {
        if (code[pos] == '\n') {
            line += 1;
        }
        pos += 1;
    }

    string tokenize(string &error) {
        error = "";
        while (true) {
            char c = get_char();

            if (c == '\0') {
                break;
            }
            else if (c == ' ' || c == '\t' || c == '\n') {
                advance();
            }
            else if (isdigit(c)) {
                string num = "";
                while (c != '\0' && isdigit(c)) {
                    num += c;
                    advance();
                    c = get_char();
                }
                tokens.push_back(Token(int_lit, num, line));
            }
            else if (isalpha(c)) {
                string words = "";
                while (c != '\0' && (isalnum(c) || c == '_')) {
                    words += c;
                    advance();
                    c = get_char();
                }
                // cout << "------" << words << "----" << endl;

                if (words == "integer") {
                    tokens.push_back(Token(int_, "", line));
                }
                else if (words == "ptr") {
                    tokens.push_back(Token(ptr, "", line));
                }
                else {
                    tokens.push_back(Token(id_, words, line));
                }
            }
            else if (c == ':') {
                tokens.push_back(Token(assign, string(1, c), line));
                advance();
            }
            else if (c == '+') {
                tokens.push_back(Token(pls, string(1, c), line));
                advance();
            }
            else if (c == '-') {
                tokens.push_back(Token(mns, string(1, c), line));
                advance();
            }
            else if (c == '*') {
                tokens.push_back(Token(mt, string(1, c), line));
                advance();
            }
            else if (c == '/') {
                tokens.push_back(Token(div_, string(1, c), line));
                advance();
            }
            else if (c == '(') {
                tokens.push_back(Token(lpr, string(1, c), line));
                advance();
            }
            else if (c == ')') {
                tokens.push_back(Token(rpr, string(1, c), line));
                advance();
            }

            // else if ( '*, /, %, ==, >=, <=, ....')

            else {
                error = "LexerError: Illegal Char : '" + string(1, c) + "' at line " + to_string(line);
                break;
            }
        }

        tokens.push_back(Token(eof, "", line));
        return error;
    }
};
#endif
