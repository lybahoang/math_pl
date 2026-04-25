// Here is a test driver for the Lexer implementation.
#include <iostream>
#include <fstream>
#include <sstream>
#include "./include/Lexer.h"
#include "./include/InvalidChar.h"
using namespace std;

// A function to read the content of a file into a string.g
string fileToString(const string& filename)
{
    ifstream file(filename);

    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + filename);
    }

    stringstream buffer;
    buffer << file.rdbuf();   // Read entire file content

    return buffer.str();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "No source program file" << endl;
    }
    else {
        try {
            Lexer lexer(fileToString(argv[1]));

            // lấy toàn bộ token
            vector<Token> tokens = lexer.tokenize();

            // in ra màn hình
            lexer.displayTokens(tokens);
        }
        catch (const InvalidCharacter& e) {
            cout << "Lexer Error: " << e.what() << endl;
        }
    }

    return 0;
}