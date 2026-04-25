/** 
    The header file for Lexer, which may include an enumeration for
    the types of tokens, a class for Lexer, and a struct/class for
    Token (if using class, then it shall provide accessor methods).
*/
#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include "Token.h"
using namespace std;

/*
Lexer: thực hiện phân tích từ vựng (lexical analysis)
- Đọc từng ký tự của source code
- Nhận diện và tách thành các token (keyword, identifier, number, operator, symbol)
- Trả về danh sách token cho Parser
- Phát hiện và báo lỗi nếu gặp ký tự không hợp lệ
*/

class Lexer {
private:
    string source;        // Lưu toàn bộ source code đầu vào
    size_t pos;           // Vị trí hiện tại trong chuỗi type size_t chuyển dùng cho index
    char currentChar;     // Ký tự đang được xét

    // Di chuyển sang ký tự tiếp theo trong source code
    void advance();

    // Bỏ qua khoảng trắng, tab, xuống dòng (không phải token)
    void skipWhitespace();

    // Đọc một số (integer hoặc float) từ source code
    // Ví dụ: 10, 3.14 → trả về token NUMBER
    Token readNumber();

    // Đọc chuỗi chữ để xác định IDENTIFIER hoặc KEYWORD
    // Nếu là "let", "print" → KEYWORD
    // Ngược lại → IDENTIFIER (x, y, sin, cos,...)
    Token readIdentifier();

    // Kiểm tra ký tự có phải toán tử không (+ - * / ^ =)
    bool isOperator(char ch);

    // Đọc toán tử và trả về token tương ứng
    Token readOperator();

    // Kiểm tra ký tự có phải ký hiệu không ( ( ) ; , )
    bool isSymbol(char ch);

    // Đọc ký hiệu và trả về token tương ứng
    Token readSymbol();

    // Trả về token kết thúc file (EOF)
    Token readEndOfFile();

public:
    // Constructor: nhận source code đầu vào và khởi tạo Lexer
    explicit Lexer(const string& input);

    // Hàm quan trọng nhất:
    // Trả về token tiếp theo mỗi lần gọi
    Token getNextToken();

    // Gọi getNextToken() nhiều lần để lấy toàn bộ token
    // Trả về danh sách token dưới dạng vector
    vector<Token> tokenize();

    // In danh sách token ra màn hình để debug/demo
    void displayTokens(const vector<Token>& tokens) const;
};

#endif