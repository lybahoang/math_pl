#include "./include/Parser.h"
#include "./include/Lexer.h"
#include "./include/InvalidChar.h"
#include "./include/Interpreter.h" 
#include <sstream>
#include <iostream>
#include <fstream>

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
    ss << file.rdbuf(); 
    return ss.str();    
}

int main(int argc, char * argv[]) 
{
    if (argc < 2)
    {
        cout << "No source program file" << endl;
    }
    else
    {
        try
        {
            
            Lexer lexer(fileToString(argv[1]));
            vector<Token> tokens = lexer.tokenize();
            Parser parser(tokens);
            vector<ASTNodePtr> programAST = parser.parse(); 
            Interpreter interpreter;
            interpreter.interpret(programAST);
        }
        catch (const InvalidCharacter &e)
        {
            cerr << "Lexical Error: " << e.what() << '\n';
        }
        catch (const Parser::InvalidSyntax &e)
        {
            cerr << "Syntax Error: " << e.what() << '\n';
        }
        catch (const InterpreterError &e) 
        {
            cerr << "Interpreter Error: " << e.what() << '\n';
        }
        catch (const exception &e) 
        {
            cerr << "Unexpected Error: " << e.what() << '\n';
        }
    }

    return 0;
}