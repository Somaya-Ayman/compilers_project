#include "parser.h"
#include <qmessagebox.h>


// Constructor
Parser::Parser(const QList<Token> &tokens) : tokens(tokens), currentIndex(0) {}

// Helper functions
Token Parser::currentToken() const {
    return currentIndex < tokens.size() ? tokens[currentIndex] : Token("", TokenType::UNKNOWN);
}

void Parser::advance() {
    if (currentIndex < tokens.size()) {
        currentIndex++;
    }
}

void Parser::match(TokenType expectedType) {
    qDebug() << "Matching token. Expected:" << Token::tokenTypeToString(expectedType)
    << ", Found:" << currentToken().toString();

    if (currentToken().type == expectedType) {
        advance();
    } else {
        QString errorMsg = QString("Unexpected token: '%1', expected: '%2'").arg(currentToken().value).arg(Token::tokenTypeToString(expectedType));
        QMessageBox::critical(nullptr, "parsing Error", errorMsg);
        throw std::runtime_error(QString("1Unexpected token: '%1', expected: '%2'")
                                     .arg(currentToken().value)
                                     .arg(Token::tokenTypeToString(expectedType))
                                     .toStdString());
    }
}


SyntaxTreeNode* Parser::parse() {
    SyntaxTreeNode* root = parseProgram(); // Use parseProgram as the entry point

    // Check if there are unparsed tokens remaining
    if (currentIndex < tokens.size()) {
        QString errorMsg = QString("Unexpected token: '%1' at position %2. Expected end of input.").arg(currentToken().value).arg(currentIndex);
        QMessageBox::critical(nullptr, "parsing Error", errorMsg);
        throw std::runtime_error(
            QString("2Unexpected token: '%1' at position %2. Expected end of input.")
                .arg(currentToken().value)
                .arg(currentIndex)
                .toStdString()
            );
    }

    return root;
}



SyntaxTreeNode* Parser::parseProgram() {
    return parseStmtSequence(); // Directly return the parsed statements
}


SyntaxTreeNode* Parser::parseStmtSequence() {
    SyntaxTreeNode* firstStmt = parseStatement(); // Parse the first statement
    SyntaxTreeNode* currentStmt = firstStmt;     // Pointer to track the current sibling

    while (currentToken().type == TokenType::SEMICOLON) {
        match(TokenType::SEMICOLON);             // Consume the semicolon
        qDebug() << "current node:" << currentStmt->name ;
        SyntaxTreeNode* nextStmt = parseStatement(); // Parse the next statement
        if (currentStmt) {
            currentStmt->sibling = nextStmt;     // Link the sibling
            currentStmt = nextStmt;             // Move to the next sibling
            qDebug() << "sibling node:" << currentStmt->name  ;
        }
    }


    // If we encounter an unexpected token, throw an error
    if (currentToken().type != TokenType::END &&
        currentToken().type != TokenType::ELSE &&
        currentToken().type != TokenType::UNTIL &&
        currentToken().type != TokenType::UNKNOWN) {
        QString errorMsg = QString("Unexpected token in statement sequence: '%1' at position %2").arg(currentToken().value).arg(currentIndex);
        QMessageBox::critical(nullptr, "parsing Error", errorMsg);
        throw std::runtime_error(
            QString("3Unexpected token in statement sequence: '%1' at position %2")
                .arg(currentToken().value)
                .arg(currentIndex)
                .toStdString()
            );
    }

    return firstStmt; // Return the first statement in the sequence
}




SyntaxTreeNode* Parser::parseStatement() {
    switch (currentToken().type) {
    case TokenType::IF:
        return parseIfStmt();
    case TokenType::REPEAT:
        return parseRepeatStmt();
    case TokenType::IDENTIFIER:{
        QString identifier = currentToken().value; // Capture the identifier's value
        return parseAssignStmt(identifier);
    }
    case TokenType::READ:
    {
        QString identifier = currentToken().value; // Capture the identifier's value
        return parseReadStmt();
    }
    case TokenType::WRITE:
        return parseWriteStmt();
    default:
        QString errorMsg = QString("Unexpected token in statement sequence: '%1' at position %2").arg(currentToken().value).arg(currentIndex);
        QMessageBox::critical(nullptr, "parsing Error", errorMsg);
        throw std::runtime_error(
            QString("4Unexpected token in statement: '%1' at position %2")
                .arg(currentToken().value)
                .arg(currentIndex)
                .toStdString()
            );
    }
}

SyntaxTreeNode* Parser::parseIfStmt() {
    SyntaxTreeNode* node = new SyntaxTreeNode("if");

    match(TokenType::IF);
    node->children.append(parseExp());
    match(TokenType::THEN);
    node->children.append(parseStmtSequence());

    if (currentToken().type == TokenType::ELSE) {
        match(TokenType::ELSE);
        node->children.append(parseStmtSequence());
    }

    match(TokenType::END);
    return node;
}

SyntaxTreeNode* Parser::parseRepeatStmt() {
    SyntaxTreeNode* node = new SyntaxTreeNode("repeat");

    match(TokenType::REPEAT);
    node->children.append(parseStmtSequence());
    match(TokenType::UNTIL);
    node->children.append(parseExp());

    return node;
}


SyntaxTreeNode* Parser::parseAssignStmt(QString identifier) {
    SyntaxTreeNode* node = new SyntaxTreeNode(QString("assign (%1)").arg(identifier));
    //node->children.append(new SyntaxTreeNode(currentToken().value)); // Identifier
    match(TokenType::IDENTIFIER);
    match(TokenType::ASSIGN);
    node->children.append(parseExp());

    return node;
}

SyntaxTreeNode* Parser::parseReadStmt() {
    match(TokenType::READ);
    SyntaxTreeNode* node = new SyntaxTreeNode(QString("read (%1)").arg(currentToken().value));
    //node->children.append(new SyntaxTreeNode(currentToken().value)); // Identifier
    match(TokenType::IDENTIFIER);

    return node;
}

SyntaxTreeNode* Parser::parseWriteStmt() {
    SyntaxTreeNode* node = new SyntaxTreeNode("write");

    match(TokenType::WRITE);
    node->children.append(parseExp());

    return node;
}

SyntaxTreeNode* Parser::parseExp() {
    SyntaxTreeNode* node = parseSimpleExp();

    if (currentToken().type == TokenType::LESSTHAN || currentToken().type == TokenType::EQUAL) {
        SyntaxTreeNode* compNode = parseComparisonOp();
        compNode->children.append(node);
        compNode->children.append(parseSimpleExp());
        node = compNode;
    }
    return node;
}

SyntaxTreeNode* Parser::parseSimpleExp() {
    SyntaxTreeNode* node = parseTerm();

    while (currentToken().type == TokenType::PLUS || currentToken().type == TokenType::MINUS) {
        SyntaxTreeNode* opNode = parseAddOp();
        opNode->children.append(node);
        opNode->children.append(parseTerm());
        node = opNode;
    }

    return node;
}

SyntaxTreeNode* Parser::parseTerm() {
    SyntaxTreeNode* node = parseFactor();

    while (currentToken().type == TokenType::MULT || currentToken().type == TokenType::DIV) {
        SyntaxTreeNode* opNode = parseMulOp();
        opNode->children.append(node);
        opNode->children.append(parseFactor());
        node = opNode;
    }

    return node;
}

SyntaxTreeNode* Parser::parseFactor() {
    if (currentToken().type == TokenType::OPENBRACKET) {
        match(TokenType::OPENBRACKET);
        SyntaxTreeNode* node = parseExp();
        match(TokenType::CLOSEDBRACKET);
        return node;
    } else if (currentToken().type == TokenType::NUMBER || currentToken().type == TokenType::IDENTIFIER) {
        SyntaxTreeNode* node = new SyntaxTreeNode(currentToken().value);
        match(currentToken().type);
        return node;
    } else {
        QString errorMsg = QString("Unexpected token in factor: '%1' at position %2").arg(currentToken().value).arg(currentIndex);
        QMessageBox::critical(nullptr, "parsing Error", errorMsg);
        throw std::runtime_error(
            QString("6Unexpected token in factor: '%1' at position %2")
                .arg(currentToken().value)
                .arg(currentIndex)
                .toStdString()
            );
    }
}

SyntaxTreeNode* Parser::parseComparisonOp() {
    SyntaxTreeNode* node = new SyntaxTreeNode(currentToken().value);
    match(currentToken().type);
    return node;
}

SyntaxTreeNode* Parser::parseAddOp() {
    SyntaxTreeNode* node = new SyntaxTreeNode(currentToken().value);
    match(currentToken().type);
    return node;
}

SyntaxTreeNode* Parser::parseMulOp() {
    SyntaxTreeNode* node = new SyntaxTreeNode(currentToken().value);
    match(currentToken().type);
    return node;
}
