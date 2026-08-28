#ifndef PARSER_CPP
#define PARSER_CPP

#include <string>
#include <vector>
#include "token.cpp"
#include "node.cpp"
using namespace std;

class Parser {
public:
    vector<Token> tokens;
    int pos;
    string error;

    Parser(vector<Token> tokens) {
        this->tokens = tokens;
        pos = 0;
        error = "";
    }

    Token* currentTok() {
        if (pos < (int)tokens.size()) {
            return &tokens[pos];
        }
        return nullptr;
    }

    void advance() {
        pos += 1;
    }

    // grammars: e --> t ( +|- t)*
    //           t --> f ( *| / f)*
    //           f --> number| identifier | (e) e.g. -- ( d - 10 )
    Node* factor() {
        Token* tok = currentTok();
        if (tok == nullptr) {
            error = "Unexpected end of input";
            return nullptr;
        }

        if (tok->type_ == int_lit) {
            string node = tok->value;
            advance();
            return new IntNode(node);
        }
        else if (tok->type_ == id_) {
            string node = tok->value;
            advance();
            return new IdNode(node);
        }
        // +a
        else if (tok->type_ == lpr) {
            advance();
            Node* node = expr();
            if (error != "") return nullptr;

            if (currentTok() != nullptr && currentTok()->type_ == rpr) {
                advance();
                return node;
            } else {
                advance();
                error = "Unexpected token: " + tok->value + ", expected ')' ";
                return nullptr;
            }
        }
        else {
            error = "Unexpected token: " + tok->toString();
            return nullptr;
        }
    }

    // term --> factor { (* | /) factor }
    Node* term() {
        Node* left = factor();
        if (error != "") return nullptr;

        while (currentTok() != nullptr && (currentTok()->type_ == mt || currentTok()->type_ == div_)) {
            string op = currentTok()->type_;
            advance();
            Node* right = factor();
            if (error != "") return nullptr;
            left = new BinOpNode(left, op, right);
        }
        return left;
    }

    // expression ---> term { (+ | - ) term }
    Node* expr() {
        Node* left = term();
        if (error != "") return nullptr;

        while (currentTok() != nullptr && (currentTok()->type_ == pls || currentTok()->type_ == mns)) {
            string op = currentTok()->type_;
            advance();
            Node* right = term();
            if (error != "") return nullptr;
            left = new BinOpNode(left, op, right);
        }
        return left;
    }

    Node* parse1() {
        return expr();
    }

    // added grammars:
    // program ---> statements:
    Node* program() {
        vector<Node*> stmts;
        while (currentTok() != nullptr && currentTok()->type_ != eof) {
            Node* s = stmt();
            if (error != "") return nullptr;
            stmts.push_back(s);
        }
        return new ProgNode(stmts);
    }

    // statements --> dec_stmt| assign_stmt| prt_stmt | expr
    Node* stmt() {
        Token* tok = currentTok();
        if (tok->type_ == int_) {
            return dec_stmt();
        }
        else if (tok->type_ == id_) {
            return assign_stmt(); // what if its just an expression.... just call expr()
        }
        else if (tok->type_ == ptr) {
            return prt_stmt();
        }
        else {
            error = "Unexpected token: " + tok->value;
            return nullptr;
        }
    }

    // dec_stmt --> 'integer' identifier | 'integer' identifier : expr
    // int a
    // int a = 10
    Node* dec_stmt() {
        // integer a : 10
        advance(); // since this token is 'integer' already checked on stmt
        Token* tok = currentTok();
        if (tok == nullptr || tok->type_ != id_) {
            error = "Unexpected token: " + (tok ? tok->value : string("EOF"));
            return nullptr;
        }

        string id = tok->value;
        advance();
        if (currentTok() != nullptr && currentTok()->type_ == assign) {
            advance();
            Node* value = expr();
            if (error != "") return nullptr;
            return new DecNode("integer", id, value);
        }

        return new DecNode("integer", id);
    }

    // assign_stmt --> identifier : expr
    // a : 10, a = 10 , a = b + c
    // a + b * c - d
    Node* assign_stmt() {
        string id = currentTok()->value; // taking the identifier as id
        advance();
        if (currentTok() != nullptr && currentTok()->type_ == assign) {

            advance(); // skipping the :
            Node* value = expr();
            if (error != "") return nullptr;
            return new AssignNode(id, value);
        }
        else {
            // a + 10
            if (currentTok() != nullptr &&
                (currentTok()->type_ == pls || currentTok()->type_ == mns ||
                 currentTok()->type_ == mt  || currentTok()->type_ == div_)) {
                pos -= 1; // since i just skipped id by calling advance....
                return expr();
            }
            else {
                error = "Unexpected token: " + (currentTok() ? currentTok()->value : string("EOF"));
                return nullptr;
            }
        }
    }

    // prt_stmt --> prt expr
    Node* prt_stmt() {
        advance(); // this token is 'ptr' already checked on stmt...
        Node* e = expr();
        if (error != "") return nullptr;
        return new PrtNode(e);
    }

    // parse tree:
    Node* parse() {
        return program();
    }
};

#endif
