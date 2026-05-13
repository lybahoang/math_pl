#include "./include/Parser.h"
#include "./include/Lexer.h"
#include "./include/InvalidChar.h"
#include "./include/Interpreter.h" 
#include <sstream>
#include <iostream>
#include <fstream>
using namespace std;

// Function prototype.
string fileToString(const string &filePath);

// main
int main(int argc, char * argv[]) 
{
    if (argc < 2) {
        cout << "No source program file" << endl;
    }
    else {
        try {
            // LEXER: Analyze and take tokens
            Lexer lexer(fileToString(argv[1]));
            vector<Token> tokens = lexer.tokenize();

            // PARSER: Analyze syntax and build abstract syntax trees.
            Parser parser(tokens);
            vector<ASTNodePtr> programAST = parser.parse(); 

            // INTERPRETER: Analyze sementic and execute the code.
            Interpreter interpreter;
            interpreter.interpret(programAST);
        }
        catch (const InvalidCharacter &e) {
            cerr << "Lexical Error: " << e.what() << '\n';
        }
        catch (const Parser::InvalidSyntax &e) {
            cerr << "Syntax Error: " << e.what() << '\n';
        }
        catch (const InterpreterError &e)  {
            cerr << "Interpreter Error: " << e.what() << '\n';
        }
        catch (const exception &e)  {
            cerr << "Unexpected Error: " << e.what() << '\n';
        }
    }

    return 0;
}

/** ---------------------------------------------------------------------------
    The function to convert the contents of a text file into a string of
    characters.
    @param  filePath    The path to a file
    @return A string containing all characters in the file.
 ------------------------------------------------------------------------------ */
string fileToString(const string &filePath)
{
    // Open the file and check if it was opened successfullt.
    ifstream file(filePath);
    if (!file.is_open())
    {
        cerr << "Error: Cannot open file\n";
        exit(EXIT_FAILURE);
    }

    // Convert the file contents into a string of characters.
    ostringstream ss;
    ss << file.rdbuf(); 

    return ss.str();    
}