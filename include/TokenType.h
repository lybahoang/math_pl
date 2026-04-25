#ifndef TOKENTYPE_H
#define TOKENTYPE_H

enum class TokenType { // chứa các type của token
    LET,
    PRINT,
    IDENTIFIER,
    NUMBER,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    CARET,
    EQUAL,
    LPAREN,
    RPAREN,
    SEMICOLON,
    COMMA,
    EOF_TOKEN
};

#endif