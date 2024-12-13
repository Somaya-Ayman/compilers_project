#include "token.h"
#include <stdexcept>
#include <qmessagebox.h>

// Constructor
Token::Token(QString value, TokenType type) : value(std::move(value)), type(type) {}

// Convert token type to string
QString Token::tokenTypeToString(TokenType type) {
    switch (type) {
    case TokenType::SEMICOLON: return "SEMICOLON";
    case TokenType::IF: return "IF";
    case TokenType::THEN: return "THEN";
    case TokenType::ELSE: return "ELSE";
    case TokenType::END: return "END";
    case TokenType::REPEAT: return "REPEAT";
    case TokenType::UNTIL: return "UNTIL";
    case TokenType::IDENTIFIER: return "IDENTIFIER";
    case TokenType::ASSIGN: return "ASSIGN";
    case TokenType::READ: return "READ";
    case TokenType::WRITE: return "WRITE";
    case TokenType::LESSTHAN: return "LESSTHAN";
    case TokenType::EQUAL: return "EQUAL";
    case TokenType::PLUS: return "PLUS";
    case TokenType::MINUS: return "MINUS";
    case TokenType::MULT: return "MULT";
    case TokenType::DIV: return "DIV";
    case TokenType::OPENBRACKET: return "OPENBRACKET";
    case TokenType::CLOSEDBRACKET: return "CLOSEDBRACKET";
    case TokenType::NUMBER: return "NUMBER";
    default: return "UNKNOWN";
    }
}

// Token to string for debugging
QString Token::toString() const {
    return value + ", " + tokenTypeToString(type);
}



QList<Token> tokenize(const QString &input) {
    QList<Token> tokens;
    int length = input.length();
    int i = 0;

    while (i < length) {
        QChar currentChar = input[i];

        // Skip whitespace
        if (currentChar.isSpace()) {
            i++;
            continue;
        }

        // Handle single tokens
        if (currentChar == ';') tokens.append(Token(";", TokenType::SEMICOLON));
        else if (currentChar == '<') tokens.append(Token("<", TokenType::LESSTHAN));
        else if (currentChar == '=') tokens.append(Token("=", TokenType::EQUAL));
        else if (currentChar == '+') tokens.append(Token("+", TokenType::PLUS));
        else if (currentChar == '-') tokens.append(Token("-", TokenType::MINUS));
        else if (currentChar == '*') tokens.append(Token("*", TokenType::MULT));
        else if (currentChar == '/') tokens.append(Token("/", TokenType::DIV));
        else if (currentChar == '(') tokens.append(Token("(", TokenType::OPENBRACKET));
        else if (currentChar == ')') tokens.append(Token(")", TokenType::CLOSEDBRACKET));

        // Handle :=
        else if (currentChar == ':' && i + 1 < length && input[i + 1] == '=') {
            tokens.append(Token(":=", TokenType::ASSIGN));
            i++; // Consume the '='
        }

        // Handle keywords and identifiers
        else if (currentChar.isLetter()) {
            QString word;
            while (i < length && (input[i].isLetter())) {
                word += input[i];
                i++;
            }
            i--; // decrement index as it will be incremented by the loop

            // Match against keywords
            if (word == "if") tokens.append(Token(word, TokenType::IF));
            else if (word == "then") tokens.append(Token(word, TokenType::THEN));
            else if (word == "else") tokens.append(Token(word, TokenType::ELSE));
            else if (word == "end") tokens.append(Token(word, TokenType::END));
            else if (word == "repeat") tokens.append(Token(word, TokenType::REPEAT));
            else if (word == "until") tokens.append(Token(word, TokenType::UNTIL));
            else if (word == "read") tokens.append(Token(word, TokenType::READ));
            else if (word == "write") tokens.append(Token(word, TokenType::WRITE));
            else tokens.append(Token(word, TokenType::IDENTIFIER));
        }

        // Handle numbers
        else if (currentChar.isDigit()) {
            QString number;
            while (i < length && input[i].isDigit()) {
                number += input[i];
                i++;
            }
            i--; // Adjust index as it will be incremented by the loop
            tokens.append(Token(number, TokenType::NUMBER));
        }

        // Handle unknown tokens
        else {
            QString errorMsg = QString("Unknown token found: '%1' at position %2").arg(currentChar).arg(i);
            QMessageBox::critical(nullptr, "Tokenization Error", errorMsg);
            return {}; // Return an empty list to indicate failure
        }

        i++;
    }

    return tokens;
}
