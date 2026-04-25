#include "./include/Parser.h"
#include "./include/Lexer.h"
#include "./include/InvalidChar.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

string fileToString(const string &filePath)
{
    ifstream file(filePath);
    if (!file.is_open())
    {
        cerr << "Error: Cannot open file\n";
        exit(EXIT_FAILURE);
    }

    ostringstream ss;
    ss << file.rdbuf(); // Read the entire file buffer into the stream
    return ss.str();    // Convert stream to string
}

int main(int argc, char * argv[]) 
{
    // std::string filepath = "H:/My Drive/University/TROYCS/8 Spring 2026 (2025.2)/CS 370/project/version1/test_cases/source3.dsl";

    if (argc < 2)
    {
        cout << "No source program file" << endl;
    }
    else
    {
        try
        {
            Lexer lexer(fileToString(argv[1]));

            // LEXER
            vector<Token> tokens = lexer.tokenize();
            lexer.displayTokens(tokens);
            cout << endl;

            // PARSER
            Parser parser(tokens);
            parser.displayAST();
        }
        catch (const InvalidCharacter &e)
        {
            cerr << e.what() << '\n';
        }
    }

    return 0;
}