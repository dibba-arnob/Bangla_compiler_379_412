#ifndef SEMANTIC_CPP
#define SEMANTIC_CPP

#include <iostream>
#include <string>
#include <vector>
#include "node.cpp"
#include "symtable.cpp"
using namespace std;

// "" ke 'None' hisebe treat kora hoy (jevabe Python e None thake)
string typeOrNone(string t) {
    return (t == "") ? "None" : t;
}

class SemanticAnalyzer {
public:
    SymbolTable symtab;
    vector<string> errors;

    void analyze(ProgNode* node) {
        for (int i = 0; i < (int)node->stmts.size(); i++) {
            analyze_stmt(node->stmts[i]);
        }
    }

    void analyze_stmt(Node* node) {
        DecNode* asDec = dynamic_cast<DecNode*>(node);       // DecNode age check, AssignNode er age
        AssignNode* asAssign = dynamic_cast<AssignNode*>(node);
        PrtNode* asPrt = dynamic_cast<PrtNode*>(node);
        IfNode* asIf = dynamic_cast<IfNode*>(node);
        WhileNode* asWhile = dynamic_cast<WhileNode*>(node);

        if (asDec != nullptr) {
            analyze_dec(asDec);
        }
        else if (asAssign != nullptr) {
            analyze_assign(asAssign);
        }
        else if (asPrt != nullptr) {
            analyze_prt(asPrt);
        }
        else if (asIf != nullptr) {
            analyze_if(asIf);
        }
        else if (asWhile != nullptr) {
            analyze_while(asWhile);
        }
        else {
            analyze_expr(node);
        }
    }

    void analyze_dec(DecNode* node) {
        // integer id [: expr(value)]
        // e.g. - integer a; integer a : 10; integer a : b + c * 10 - 2
        if (node->t != "integer") {
            string msg = "Variable '" + node->id + "' with Unknown Type '" + node->t + "'";
            errors.push_back(msg);
            return;
        }
        else {
            string err;
            bool ok = symtab.define(node->id, node->t, "", err);
            if (!ok) {
                errors.push_back(err);
                return;
            }
            if (node->val != nullptr) {
                // checks the type of the RHS
                string expr_type = analyze_expr(node->val);
                if (expr_type != "" && expr_type != "integer") {
                    string msg = "Type mismatch: '" + node->id + "' declared as 'integer' but assigned '" + expr_type + "'";
                    errors.push_back(msg);
                    return;
                }
                ok = symtab.update(node->id, node->val->toString(), err);
                if (!ok) {
                    errors.push_back(err);
                }
            }
        }
    }

    void analyze_assign(AssignNode* node) {
        // id : expr(i.e - value)
        if (symtab.is_defined(node->id)) {
            string expr_type = analyze_expr(node->val);
            if (expr_type != "" && expr_type != "integer") {
                string msg = "Type mismatch: '" + node->id + "' declared as 'integer' but assigned '" + expr_type + "'";
                errors.push_back(msg);
            }
        }
        string err;
        bool ok = symtab.update(node->id, (node->val != nullptr) ? node->val->toString() : "", err);
        if (!ok) {
            errors.push_back(err);
        }
    }

    void analyze_prt(PrtNode* node) {
        // ptr expr
        // check is this a valid expression
        analyze_expr(node->expr);
    }

    // notun: if_stmt check kore -- condition, then body, else body shob check hoy
    void analyze_if(IfNode* node) {
        string cond_type = analyze_expr(node->cond);
        if (cond_type != "" && cond_type != "integer") {
            string msg = "Type mismatch: if condition must be 'integer' but got '" + cond_type + "'";
            errors.push_back(msg);
        }

        for (int i = 0; i < (int)node->thenStmts.size(); i++) {
            analyze_stmt(node->thenStmts[i]);
        }

        if (node->hasElse) {
            for (int i = 0; i < (int)node->elseStmts.size(); i++) {
                analyze_stmt(node->elseStmts[i]);
            }
        }
    }

    // notun: while_stmt check kore -- condition, loop body shob check hoy
    void analyze_while(WhileNode* node) {
        string cond_type = analyze_expr(node->cond);
        if (cond_type != "" && cond_type != "integer") {
            string msg = "Type mismatch: while condition must be 'integer' but got '" + cond_type + "'";
            errors.push_back(msg);
        }

        for (int i = 0; i < (int)node->stmts.size(); i++) {
            analyze_stmt(node->stmts[i]);
        }
    }

    // expr --> id , number, binOp
    // return "" mane None (kono type dorkar nai / error)
    string analyze_expr(Node* node) {
        IdNode* asId = dynamic_cast<IdNode*>(node);
        IntNode* asInt = dynamic_cast<IntNode*>(node);
        BinOpNode* asBinOp = dynamic_cast<BinOpNode*>(node);

        if (asId != nullptr) {
            Symbol* x = symtab.lookup(asId->value);
            if (x == nullptr) {
                errors.push_back("Variable '" + asId->value + "' used but not declared");
                return "";
            }
            if (x->is_init == false) {
                errors.push_back("Variable '" + asId->value + "' used but not initialized");
            }
            return x->type_;
        }
        else if (asInt != nullptr) {
            return "integer";
        }
        else if (asBinOp != nullptr) {
            string left_type = analyze_expr(asBinOp->left);
            string right_type = analyze_expr(asBinOp->right);

            if (left_type != "integer" || right_type != "integer") {
                string msg = "Type mismatch: '" + asBinOp->op + "' between '" + typeOrNone(left_type) + "' and '" + typeOrNone(right_type) + "'";
                errors.push_back(msg);
                return "";
            }

            if (left_type == "integer" && right_type == "integer") {
                return "integer";
            }
            return "";
        }
        else {
            return "";
        }
    }
};

#endif
