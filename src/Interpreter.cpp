#include "../include/Interpreter.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <sstream>

Interpreter::Interpreter() {
    isInsideFunction = false;
    currentParamValue = 0.0;
}

void Interpreter::interpret(const std::vector<ASTNodePtr>& program) { 
    // Execute each statement in the AST program.
    try {
        for (const auto& stmt : program) {
            executeStatement(stmt);
        }
    } 
    
    // Catch and report semantic/runtime errors during execution.
    catch (const InterpreterError& e) { 
        std::cerr << "Semantic/Runtime Error: " << e.what() << std::endl;
    }
}


/*
Executes AST statement nodes sequentially 
and updates interpreter state.
*/
void Interpreter::executeStatement(ASTNodePtr node) {
    if (!node) return;

    // Handle variable declaration statements
    if (auto varDecl = std::dynamic_pointer_cast<VariableDeclarationNode>(node)) {
        if (globalVariables.find(varDecl->name) != globalVariables.end() ||
            userFunctions.find(varDecl->name) != userFunctions.end() ||
            isBuiltInFunction(varDecl->name)) {
            throw InterpreterError("Duplicate declaration for variable/function: " + varDecl->name);
        }
        globalVariables[varDecl->name] = evaluate(varDecl->expr);
    }

    // Handle user-defined function declarations
    else if (auto funcDecl = std::dynamic_pointer_cast<FunctionDeclarationNode>(node)) {
        if (globalVariables.find(funcDecl->name) != globalVariables.end() ||
            userFunctions.find(funcDecl->name) != userFunctions.end() ||
            isBuiltInFunction(funcDecl->name)) {
            throw InterpreterError("Duplicate declaration for variable/function: " + funcDecl->name);
        }
        
        validateFunctionBody(funcDecl->expr, funcDecl->parameter);
        
        userFunctions[funcDecl->name] = {funcDecl->parameter, funcDecl->expr};
    }

    // Handle variable assignment statements
    else if (auto assign = std::dynamic_pointer_cast<AssignmentNode>(node)) {
        if (globalVariables.find(assign->name) == globalVariables.end()) {
            throw InterpreterError("Variable assigned before declaration: " + assign->name);
        }
        globalVariables[assign->name] = evaluate(assign->expr);
    }

    // Handle print statements with detailed expression output
    else if (auto printStmt = std::dynamic_pointer_cast<PrintNode>(node)) {
        std::string originExpr = expressionToString(printStmt->expr);
        std::cout << originExpr << " = " << evaluate(printStmt->expr) << std::endl;
    }

    // Evaluate standalone expressions
    else {
        evaluate(node);
    }
}


/*
Recursively evaluates AST expression nodes 
and returns their numeric value.
*/
double Interpreter::evaluate(ASTNodePtr node) {

    // Handle numeric literal nodes
    if (auto num = std::dynamic_pointer_cast<NumberNode>(node)) {
        return num->value;
    }

    // Handle identifier/variable nodes
    if (auto id = std::dynamic_pointer_cast<IdentifierNode>(node)) {
        if (isInsideFunction && id->name == currentParamName) {
            return currentParamValue;
        }
        if (globalVariables.find(id->name) != globalVariables.end()) {
            return globalVariables[id->name];
        }
        throw InterpreterError("Identifier used before declaration: " + id->name);
    }

    // Handle binary arithmetic operations
    if (auto binOp = std::dynamic_pointer_cast<BinaryOperationNode>(node)) {
        double left = evaluate(binOp->left);
        double right = evaluate(binOp->right);
        
        if (binOp->op == "+") return left + right;
        if (binOp->op == "-") return left - right;
        if (binOp->op == "*") return left * right;
        if (binOp->op == "/") {
            // Handle division with runtime divide-by-zero checking
            if (right == 0.0) throw InterpreterError("Divide-by-zero error.");
            return left / right;
        }
        // Handle exponentiation
        if (binOp->op == "^") return std::pow(left, right);
    }

    // Handle unary operations
    if (auto unOp = std::dynamic_pointer_cast<UnaryOperationNode>(node)) {
        double val = evaluate(unOp->expr);
        if (unOp->op == "-") return -val;
        if (unOp->op == "+") return val;
    }

    // Handle function call expressions
    if (auto fCall = std::dynamic_pointer_cast<FunctionCallNode>(node)) {
        std::vector<double> args;
        for (auto argNode : fCall->arguments) {
            args.push_back(evaluate(argNode));
        }

        if (isBuiltInFunction(fCall->name)) {
            return callBuiltInFunction(fCall->name, args);
        }

        if (userFunctions.find(fCall->name) != userFunctions.end()) {
            if (args.size() != 1) {
                throw InterpreterError("Function " + fCall->name + " expects exactly 1 argument.");
            }
            
            bool prevInside = isInsideFunction;
            std::string prevParam = currentParamName;
            double prevVal = currentParamValue;
            
            isInsideFunction = true;
            currentParamName = userFunctions[fCall->name].parameter;
            currentParamValue = args[0];
            
            double result = evaluate(userFunctions[fCall->name].body);
            
            isInsideFunction = prevInside;
            currentParamName = prevParam;
            currentParamValue = prevVal;
            
            return result;
        }
        throw InterpreterError("Function not declared: " + fCall->name);
    }
    
    throw InterpreterError("Unknown expression node.");
}


/*
Recursively validates identifiers used inside a function body.
Ensures all variables are either the function parameter
or previously declared global variables.
*/
void Interpreter::validateFunctionBody(ASTNodePtr node, const std::string& paramName) {
    if (!node) return;
    
    // Handle identifier nodes
    if (auto idNode = std::dynamic_pointer_cast<IdentifierNode>(node)) {
        if (idNode->name != paramName && globalVariables.find(idNode->name) == globalVariables.end()) {
            throw InterpreterError("Incompatible variable '" + idNode->name + "' used in function declaration.");
        }

    // Handle binary operation nodes recursively
    } else if (auto binOp = std::dynamic_pointer_cast<BinaryOperationNode>(node)) {
        validateFunctionBody(binOp->left, paramName);
        validateFunctionBody(binOp->right, paramName);
    } 
    
    // Handle unary operation nodes recursively
    else if (auto unOp = std::dynamic_pointer_cast<UnaryOperationNode>(node)) {
        validateFunctionBody(unOp->expr, paramName);
    } 
    // Handle function call nodes recursively
    else if (auto fCall = std::dynamic_pointer_cast<FunctionCallNode>(node)) {
        for (auto arg : fCall->arguments) {
            validateFunctionBody(arg, paramName);
        }
    }
}

/*
Checks whether a function name is a supported built-in function.
*/
bool Interpreter::isBuiltInFunction(const std::string& name) {
    return name == "sin" || name == "cos" || name == "sqrt" || 
           name == "abs" || name == "ln" || name == "min" || name == "max" ||
           name == "log" || name == "log2";
}


/*
Executes supported built-in functions with runtime argument validation.
*/
double Interpreter::callBuiltInFunction(const std::string& name, const std::vector<double>& args) {
    
    // Handle functions requiring two arguments
    if (name == "min" || name == "max") {
        if (args.size() != 2) throw InterpreterError(name + " expects 2 arguments.");
        if (name == "min") return std::min(args[0], args[1]);
        if (name == "max") return std::max(args[0], args[1]);
    } 

    // Handle single-argument functions
    else {
        if (args.size() != 1) throw InterpreterError(name + " expects 1 argument.");
        if (name == "sin") return std::sin(args[0]);
        if (name == "cos") return std::cos(args[0]);
        if (name == "sqrt") {
            if (args[0] < 0) throw InterpreterError("Square root of a negative number.");
            return std::sqrt(args[0]);
        }
        if (name == "abs") return std::abs(args[0]);
        if (name == "log") {
            if (args[0] <= 0) throw InterpreterError("Logarithm of non-positive number.");
            return std::log10(args[0]);
        }
        if (name == "ln") {
            if (args[0] <= 0) throw InterpreterError("Logarithm of non-positive number.");
            return std::log(args[0]);
        }
        if (name == "log2") {
            if (args[0] <= 0) throw InterpreterError("Logarithm of non-positive number.");
            return std::log2(args[0]);
        }
    }
    return 0.0;
}

/*
Helper function to convert AST expression nodes 
back into their string representation for display purposes.
*/
std::string Interpreter::expressionToString(ASTNodePtr node) {
    if (!node) return "";
    
    // Handle numeric literal nodes
    if (auto num = std::dynamic_pointer_cast<NumberNode>(node)) {
        std::ostringstream out;
        out << num->value; 
        return out.str();
    }
    
    // Handle identifier/variable nodes
    if (auto id = std::dynamic_pointer_cast<IdentifierNode>(node)) {
        return id->name;
    }
    
    // Handle binary arithmetic operations recursively
    if (auto binOp = std::dynamic_pointer_cast<BinaryOperationNode>(node)) {
        return "(" + expressionToString(binOp->left) + " " + binOp->op + " " + expressionToString(binOp->right) + ")";
    }
    
    // Handle unary operations recursively
    if (auto unOp = std::dynamic_pointer_cast<UnaryOperationNode>(node)) {
        return unOp->op + expressionToString(unOp->expr);
    }
    
    // Handle function call expressions recursively
    if (auto fCall = std::dynamic_pointer_cast<FunctionCallNode>(node)) {
        std::string s = fCall->name + "(";
        for (size_t i = 0; i < fCall->arguments.size(); ++i) {
            s += expressionToString(fCall->arguments[i]);
            if (i < fCall->arguments.size() - 1) s += ", ";
        }
        return s + ")";
    }
    
    return "?";
}
