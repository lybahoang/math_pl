#include <string>
#include <memory>
#include <iostream>
#include <iomanip>
#include <vector>
#ifndef AST_NODE_H
#define AST_NODE_H

/* An abstract base class for a node in the abstract syntax tree */
class ASTNode {
public:
    virtual void print(int indent = 0) = 0; // For demonstration.
    virtual ~ASTNode() { };
};

using ASTNodePtr = std::shared_ptr<ASTNode>;

/** ---------------------------------------------------------------------------
    The class to represent an abstract syntax tree for binary operators
    expression, which include add_expression, mul_expression, pow_expression.
 ------------------------------------------------------------------------------*/
class BinaryOperationNode : public ASTNode {
public:
    std::string op;
    ASTNodePtr left, right;

    BinaryOperationNode(const std::string &op, ASTNodePtr l, ASTNodePtr r)
        : op{ op }, left{ l }, right{ r } { }

    void print(int indent) override {
        std::cout << std::setw(indent) << ""
                  << "BinaryOperationNode(" << op << ")" << std::endl;
        left->print(indent + 5);
        right->print(indent + 5);
    }
};

/** ---------------------------------------------------------------------------
    The class to represent an abstract syntax tree for unary operator
    expression.
 ------------------------------------------------------------------------------*/
class UnaryOperationNode : public ASTNode {
public:
    std::string op;
    ASTNodePtr expr;

    UnaryOperationNode(const std::string &op, ASTNodePtr expr)
        : op{ op }, expr{ expr } { }

    void print(int indent) override {
        std::cout << std::setw(indent) << ""
                  << "UnaryOperationNode(" << op << ")" << std::endl;
        expr->print(indent + 5);
    }
};

/** ---------------------------------------------------------------------------
    The class to represent an abstract syntax tree for variable declaration
    derivation.
 ------------------------------------------------------------------------------*/
class VariableDeclarationNode : public ASTNode {
public:
    std::string name;
    ASTNodePtr expr;

    VariableDeclarationNode(const std::string &name, ASTNodePtr expr)
        : name{ name }, expr{ expr } { }

    void print(int indent) override {
        std::cout << std::setw(indent) << ""
                  << "VariableDeclarationNode(" << name << ")" << std::endl;
        expr->print(indent + 5);
    }
};

/** ---------------------------------------------------------------------------
    The class to represent an abstract syntax tree for a variable assignment
    derivation.
 ------------------------------------------------------------------------------*/
class AssignmentNode : public ASTNode {
public:
    std::string name;
    ASTNodePtr expr;

    AssignmentNode(const std::string &name, ASTNodePtr expr)
        : name{ name }, expr{ expr } { }

    void print(int indent) override {
        std::cout << std::setw(indent) << ""
                  << "AssignmentNode(" << name << ")" << std::endl;
        expr->print(indent + 5);
    }
};

/** ---------------------------------------------------------------------------
    The class to represent an abstract syntax tree for a function declaration.
 ------------------------------------------------------------------------------*/
class FunctionDeclarationNode : public ASTNode {
public:
    std::string name;
    std::string parameter;
    ASTNodePtr expr;

    FunctionDeclarationNode(const std::string &name,
                            const std::string &para,
                            ASTNodePtr expr)
        : name{ name }, parameter{ para }, expr{ expr } { }

    void print(int indent) override {
        std::cout << std::setw(indent) << ""
                  << "FunctionDeclarationNode(" << name
                  << ", para: " << parameter << ")" << std::endl;
        expr->print(indent + 5);
    }
};

/** ---------------------------------------------------------------------------
    The class to represent an abstract syntax tree for a print statement.
 ------------------------------------------------------------------------------*/
class PrintNode : public ASTNode {
public:
    ASTNodePtr expr;

public:
    PrintNode(ASTNodePtr expr) : expr{ expr } { }

    void print(int indent) override {
        std::cout << std::setw(indent) << ""
                  << "PrintNode" << std::endl;
        expr->print(indent + 5);
    }
};

/** ---------------------------------------------------------------------------
    The class to represent an abstract syntax tree for an identifier.
 ------------------------------------------------------------------------------*/
class IdentifierNode : public ASTNode {
public:
    std::string name;

    IdentifierNode(const std::string &name) : name{ name } { }

    void print(int indent) override {
        std::cout << std::setw(indent) << ""
                  << "IdentifierNode(" << name << ")" << std::endl;
    }
};

/** ---------------------------------------------------------------------------
    The class to represent an abstract syntax tree for a number.
 ------------------------------------------------------------------------------*/
class NumberNode : public ASTNode {
public:
    double value;

    NumberNode(double v) : value{ v } { }

    void print(int indent) override {
        std::cout << std::setw(indent) << "" 
                  << "NumberNode(" << value << ")" << std::endl;
    }
};

/** ---------------------------------------------------------------------------
    The class to represent an abstract syntax tree for a function call.
 ------------------------------------------------------------------------------*/
class FunctionCallNode : public ASTNode {
public:
    std::string name;
    std::vector<ASTNodePtr> arguments;

    FunctionCallNode(const std::string &name, std::vector<ASTNodePtr> args)
        : name{ name }, arguments{ args } { }

    void print(int indent) override {
        std::cout << std::setw(indent) << ""
                  << "FunctionCallNode(" << name << ")" << std::endl;
        for (const auto& arg : arguments)
            arg->print(indent + 5);
    }
};
#endif