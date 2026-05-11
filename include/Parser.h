#include <vector>
#include <stdexcept>
#include <string>
#include <iostream>
#include "ASTNode.h"
#include "Lexer.h"
#ifndef PARSER_H
#define PARSER_H

class Parser
{
private:
    std::vector<Token> tokens;
    size_t pos = 0;

    Token peek() {
        if (pos >= tokens.size()) return Token(EOF_TOKEN, "");
        return tokens[pos];
    }

    Token get() {
        if (pos > tokens.size()) return Token(EOF_TOKEN, "");
        else return tokens[pos++];
    }

    Token match(TokenType expected, const std::string &message) {
        if (expected == peek().type) return get();
        throw InvalidSyntax(message);
    }

private:
    /* Helper parsing methods */
    ASTNodePtr parseStatement();
    ASTNodePtr parseVariableDeclaration();
    ASTNodePtr parseAssignment();
    ASTNodePtr parsePrintStatement();
    ASTNodePtr parseFunctionDeclaration();

    ASTNodePtr parseExpression();
    ASTNodePtr parseAddExpression();
    ASTNodePtr parseMulExpression();
    ASTNodePtr parsePowExpression();
    ASTNodePtr parseUnaryExpression();
    ASTNodePtr parsePrimary();
    ASTNodePtr parseNumber();
    ASTNodePtr parseIdentifier();
    ASTNodePtr parseFunctionCall();
    std::vector<ASTNodePtr> parseArgList();

public:
    class InvalidSyntax {
    private:
        std::string message;
    public:
        InvalidSyntax(const std::string &m) : message{ m } { }
        std::string what() const { return "Syntax error: " + message; }
    };

public:
    Parser(const std::vector<Token> &t) : tokens{ t } { }
    std::vector<ASTNodePtr> parse();
    void displayAST(); 
};
#endif