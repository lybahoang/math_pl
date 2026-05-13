#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include "TokenType.h"
using namespace std;

class Token {

public:
    TokenType type; //loại token
    string value; // giá trị gốc đọc từ source code, ví dụ "let", "x", "10", "+"
    Token(TokenType t, const string& v) : type(t), value(v) {}
};

#endif