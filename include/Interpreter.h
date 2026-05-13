#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <stdexcept>
#include "ASTNode.h"

// Exception class used for semantic and runtime errors
class InterpreterError : public std::runtime_error {
public:
    InterpreterError(const std::string& message) : std::runtime_error(message) {}
};

// Structure for storing user-defined function information
struct FunctionDef {
    std::string parameter;
    ASTNodePtr body;
};

class Interpreter {
private:
    std::unordered_map<std::string, double> globalVariables;
    std::unordered_map<std::string, FunctionDef> userFunctions;

    // Variables used to track function scope during execution
    bool isInsideFunction;
    std::string currentParamName;
    double currentParamValue;

    // Core interpreter execution functions
    void executeStatement(ASTNodePtr node);
    double evaluate(ASTNodePtr node);
    
    // Semantic validation function:
    // Ensures function bodies only use valid parameters
    // or previously declared global variables
    void validateFunctionBody(ASTNodePtr node, const std::string& paramName);
    
    // Built-in mathematical function handlers
    bool isBuiltInFunction(const std::string& name);
    double callBuiltInFunction(const std::string& name, const std::vector<double>& args);
    
    // Helper function to enhance display function
    std::string expressionToString(ASTNodePtr node);
public:
    Interpreter();
    
    // Main public function used to execute the entire program
    void interpret(const std::vector<ASTNodePtr>& program);
};

#endif 