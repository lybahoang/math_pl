// Here is a test driver for the Lexer implementation.
#include <iostream>
#include "./include/Lexer.h"
#include "./include/InvalidChar.h"
using namespace std;

int main() {
    // input test
    string source = "let = cos(x) + y; \n print(a - b) ";

    try {
        Lexer lexer(source);

        // lấy toàn bộ token
        vector<Token> tokens = lexer.tokenize();

        // in ra màn hình
        lexer.displayTokens(tokens);
    }
    catch (const InvalidCharacter& e) {
        cout << "Lexer Error: " << e.what() << endl;
    }

    return 0;
}