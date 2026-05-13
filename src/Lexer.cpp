/**
    The implementation file for Lexer.h. Here contains the
    definition of member functions in the class Lexer.
*/
#include "../include/Lexer.h"
#include "../include/InvalidChar.h"
#include <iostream>
#include <cctype>
#include "../include/Token.h"
using namespace std;

// Constructor: nhận source code đầu vào và khởi tạo vị trí ban đầu
Lexer::Lexer(const string& input) : source(input), pos(0) {
    currentChar = source.empty() ? '\0' : source[pos];
}

// Di chuyển sang ký tự tiếp theo
void Lexer::advance() {
    pos++;
    currentChar = (pos < source.size()) ? source[pos] : '\0';
}

// Bỏ qua khoảng trắng, tab, xuống dòng
void Lexer::skipWhitespace() {
    while (currentChar != '\0' && isspace(static_cast<unsigned char>(currentChar))) {  // isspace(x) chỉ nhận giá trị x từ 0->255
        advance();
    }
}

void Lexer::ignoreComment() {
    while (currentChar != '\n' && currentChar != '\0') {
        advance();
    }
}

// Đọc số nguyên hoặc số thực
Token Lexer::readNumber() {
    string result;
    bool hasDot = false; // Biến này dùng để kiểm tra số đã có dấu "." chưa

    while (currentChar != '\0' &&
           (isdigit(static_cast<unsigned char>(currentChar)) || currentChar == '.')) { // ép kiểu sang unsigned phòng trường hợp char <0 mặc dù những ký tự nằm ngoài unsigned nó sẽ trả về ký tự bất kỳ trong 0->255 nhưng vẫn thuôc kiểu usigned char nên isdigit() vẫn trả về false vì nó chỉ nhân giá trị từ 0->9

        if (currentChar == '.') {
            if (hasDot) {
                throw InvalidCharacter("Lexical errors: Invalid number format");
            }
            hasDot = true;
        }

        result += currentChar;
        advance();
    }

    return Token(TokenType::NUMBER, result);
}//dừng khi gặp ký tự KHÔNG hợp lệ hoặc cuối chuỗi \0

// Đọc identifier hoặc keyword
Token Lexer::readIdentifier() { //dừng khi gặp ký tự KHÔNG hợp lệ hoặc cuối chuỗi \0
    string result;

    while (currentChar != '\0' &&
           (isalnum(static_cast<unsigned char>(currentChar)) || currentChar == '_')) { //isalnum(x): kiểm tra xem x có phải chữ cái hoặc số hay không và dấu _ for ex: total_1 = 10
        result += currentChar;
        advance();
    }

    if (result == "let") {
        return Token(TokenType::LET, result);
    }

    if (result == "print") {
        return Token(TokenType::PRINT, result);
    }

    return Token(TokenType::IDENTIFIER, result);
} //dừng khi gặp ký tự KHÔNG hợp lệ hoặc cuối chuỗi \0

// Kiểm tra ký tự có phải toán tử không
bool Lexer::isOperator(char ch) {
    return ch == '+' || ch == '-' || ch == '*' ||
           ch == '/' || ch == '^' || ch == '=';
}

// Đọc toán tử và trả token tương ứng
Token Lexer::readOperator() {
    char op = currentChar;
    advance();

    switch (op) {
        case '+': return Token(TokenType::PLUS, "+");
        case '-': return Token(TokenType::MINUS, "-");
        case '*': return Token(TokenType::STAR, "*");
        case '/': return Token(TokenType::SLASH, "/");
        case '^': return Token(TokenType::CARET, "^");
        case '=': return Token(TokenType::EQUAL, "=");
        default:
            throw InvalidCharacter(string("Lexical errors: Invalid operator: ") + op);
    }
}

// Kiểm tra ký tự có phải symbol không
bool Lexer::isSymbol(char ch) {
    return ch == '(' || ch == ')' || ch == ';' || ch == ',';
}

// Đọc symbol và trả token tương ứng
Token Lexer::readSymbol() {
    char symbol = currentChar;
    advance();

    switch (symbol) {
        case '(': return Token(TokenType::LPAREN, "(");
        case ')': return Token(TokenType::RPAREN, ")");
        case ';': return Token(TokenType::SEMICOLON, ";");
        case ',': return Token(TokenType::COMMA, ",");
        default:
            throw InvalidCharacter(string("Lexical errors: Invalid symbol: ") + symbol);
    }
}

// Trả token kết thúc file
Token Lexer::readEndOfFile() {
    return Token(TokenType::EOF_TOKEN, "");
}

// Trả về token tiếp theo
/* currentChar
   ↓
space? → bỏ qua
   ↓
chữ? → readIdentifier
   ↓
số? → readNumber
   ↓
operator? → readOperator
   ↓
symbol? → readSymbol
   ↓
không hợp lệ → lỗi */
Token Lexer::getNextToken() {
    while (currentChar != '\0') {

        if (isspace(static_cast<unsigned char>(currentChar))) {
            skipWhitespace();
            continue;
        }

        if (currentChar == '#') {
            ignoreComment();
            continue;
        }

        if (isalpha(static_cast<unsigned char>(currentChar))) {
            return readIdentifier();
        }

        if (isdigit(static_cast<unsigned char>(currentChar))) {
            return readNumber();
        }

        if (isOperator(currentChar)) {
            return readOperator();
        }

        if (isSymbol(currentChar)) {
            return readSymbol();
        }

        throw InvalidCharacter(string("Lexical errors: Invalid character: ") + currentChar);
    }

    return readEndOfFile(); // mếu gặp \0 thì gọi hàm kết thúc file
}

// Lấy toàn bộ token trong source code
vector<Token> Lexer::tokenize() {
    vector<Token> tokens;

    Token token = getNextToken();

    while (token.type != TokenType::EOF_TOKEN) {
        tokens.push_back(token); // thêm token vừa tìm đc vào list
        token = getNextToken();  // lại goi để lấy token tiếp theo
    }

    tokens.push_back(token);
    return tokens;
}

// Chuyển TokenType thành string để in ra màn hình
static string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::LET: return "LET";
        case TokenType::PRINT: return "PRINT";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::CARET: return "CARET";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::EOF_TOKEN: return "EOF";
        default: return "UNKNOWN";
    }
}

// In danh sách token
void Lexer::displayTokens(const vector<Token>& tokens) const {
    for (const Token& token : tokens) { //không copy, dùng trực tiếp phần tử trong vector
        if (token.type == TokenType::EOF_TOKEN) {
            continue;
        }

        cout << "[ Type: " << tokenTypeToString(token.type)
             << ", value: " << token.value << " ]" << endl;
    }
}
