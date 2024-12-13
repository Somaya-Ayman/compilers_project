#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "syntaxtree.h"
#include <QList>
#include <QDebug>

class Parser {
public:
    explicit Parser(const QList<Token> &tokens);
    SyntaxTreeNode* parse();

private:
    QList<Token> tokens;
    int currentIndex;

    Token currentToken() const;
    void advance();

    void match(TokenType expectedType);
    SyntaxTreeNode* parseProgram();       // program -> stmt-sequence
    SyntaxTreeNode* parseStmtSequence(); // stmt-sequence -> statement {; statement}
    SyntaxTreeNode* parseStatement();    // statement -> if-stmt | repeat-stmt | ...
    SyntaxTreeNode* parseIfStmt();       // if-stmt -> if exp then stmt-sequence ...
    SyntaxTreeNode* parseRepeatStmt();   // repeat-stmt -> repeat stmt-sequence ...
    SyntaxTreeNode* parseAssignStmt(QString identifier);   // assign-stmt -> identifier := exp
    SyntaxTreeNode* parseReadStmt();     // read-stmt -> read identifier
    SyntaxTreeNode* parseWriteStmt();    // write-stmt -> write exp
    SyntaxTreeNode* parseExp();          // exp -> simple-exp comparison-op ...
    SyntaxTreeNode* parseSimpleExp();    // simple-exp -> term {addop term}
    SyntaxTreeNode* parseTerm();         // term -> factor {mulop factor}
    SyntaxTreeNode* parseFactor();       // factor -> (exp) | number | identifier
    SyntaxTreeNode* parseComparisonOp(); // comparison-op -> < | =
    SyntaxTreeNode* parseAddOp();        // addop -> + | -
    SyntaxTreeNode* parseMulOp();        // mulop -> * | /

};

#endif // PARSER_H
