#ifndef TOKEN_H
#define TOKEN_H

#include <QString>
#include <QList>

// Define token types
enum class TokenType {
    SEMICOLON, IF, THEN, ELSE, END, REPEAT, UNTIL, IDENTIFIER, ASSIGN, READ, WRITE,
    LESSTHAN, EQUAL, PLUS, MINUS, MULT, DIV, OPENBRACKET, CLOSEDBRACKET, NUMBER, UNKNOWN
};

// Token class
class Token {
public:
    QString value;
    TokenType type;

    Token(QString value, TokenType type);
    QString toString() const;
    static QString tokenTypeToString(TokenType type);
};

// Function to tokenize input
QList<Token> tokenize(const QString &input);

#endif // TOKEN_H
