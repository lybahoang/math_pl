#include "../include/Parser.h"

/** ---------------------------------------------------------------------------
    Definition of the parseStatement function.
    The parseStatement function implements the grammar rule 2:

    statement → variable_declaration | variable_assignment 
            | print_statement | function_declaration | expression_statement

 ------------------------------------------------------------------------------ */
ASTNodePtr Parser::parseStatement() {
    // Parse variable declaration statement or function declaration statement.
    if (peek().type == LET) {
        match(LET, "Expected 'let' in a declaration");
        if (peek().type == IDENTIFIER && pos + 1 < tokens.size() && tokens[pos + 1].type == LPAREN)
            return parseFunctionDeclaration();
        else
            return parseVariableDeclaration();
    }

    // Parse a printing statement.
    else if (peek().type == PRINT) {
        match(PRINT, "Expected 'print' in a print statement");
        return parsePrintStatement();
    }

    // Parse an assignment statement.
    else if (peek().type == IDENTIFIER && 
            pos + 1 < tokens.size() && 
            tokens[pos + 1].type == EQUAL) {
        return parseAssignment();
    }

    // Parse a standalone expressions including expression and function call.
    else return parseExpressionStatement();
}


/** ---------------------------------------------------------------------------
    Definition of the parseVariableDeclaration function.

    The parseVariableDeclaration function will check if a variable declaration
    statement is syntactically valid, and then build its abstract syntax tree.

    The function implements the grammar rule 3:
        variable_declaration → “ let ” identifier “ = ” expression “ ; ”
 ------------------------------------------------------------------------------ */
ASTNodePtr Parser::parseVariableDeclaration() {
    Token varName = match(IDENTIFIER, "Expected an identifier after in a declaration");
    match(EQUAL, "Expect '=' after variable name in variable declaration");
    ASTNodePtr expr = parseExpression();
    match(SEMICOLON, "Expected ';' at the end of a statement");

    // Create a node of VariableDeclarationNode.
    return std::make_shared<VariableDeclarationNode>(varName.value, expr);
}


/** ---------------------------------------------------------------------------
    Definition of the parseFunctionDeclaration function.

    The parseFunctionDeclaration function will validate the syntax of a
    function declaraction statement and build its abstract syntax tree.
    
    The function function implements the grammar rule 18:
        function_declaration → “ let ” identifier “ ( ” identifier “ ) ” “ = ” 
            expression “ ; ”
 ------------------------------------------------------------------------------ */
ASTNodePtr Parser::parseFunctionDeclaration() {
    Token functionName = match(IDENTIFIER, "Expected a function name in a function declaration");

    match(LPAREN, "Expected '(' in a function declaration");
    Token parameter = match(IDENTIFIER, "Expect parameter in a function declaration");
    match(RPAREN, "Expected ')' in a function declaration");

    match(EQUAL, "Expected '=' in a function declaration");
    ASTNodePtr body = parseExpression();    // Parse the function body
    match(SEMICOLON, "Expected ';' at the end of a function declaration");

    // Create a ASTNodePtr.
    return std::make_shared<FunctionDeclarationNode>(functionName.value, parameter.value, body);
}

/** ---------------------------------------------------------------------------
    Definition of the parsePrintStatement function.
    The function will validate the syntax of a print statement and build its
    abstract syntax tree.

    The function implements the grammar rule 17:
        print_statement → “ print ”  “ ( ” expression “ ) ”  “ ; ” 
 ------------------------------------------------------------------------------ */
ASTNodePtr Parser::parsePrintStatement() {
    match(LPAREN, "Expected '(' in a print statment");
    ASTNodePtr expr = parseExpression();    // Parse the expression in the print.
    match(RPAREN, "Expected a ')' in a print statement");
    match(SEMICOLON, "Expected ';' at the end of a print statement");

    return std::make_shared<PrintNode>(expr);
}


/** ---------------------------------------------------------------------------
    Definition of the parseAssignment function.
    The function will validate the syntax of a variable assignment statement
    and build its abstract syntax tree.

    The function implements the grammar rule:
        variable_assignment → identifier “ = ” expression “ ; ”
 ------------------------------------------------------------------------------ */
ASTNodePtr Parser::parseAssignment() {
    Token identifier = match(IDENTIFIER, "Expected an identifier in an assignment");
    match(EQUAL, "Expected '=' after variable name in assignment");
    ASTNodePtr expr = parseExpression();
    match(SEMICOLON, "Expected ';' at the end of an assignment");

    return std::make_shared<AssignmentNode>(identifier.value, expr);
}


/** ---------------------------------------------------------------------------
    Definition of the parseExpressionStatement function.
    The function will validate the syntax of a standalone expression statement
    and build its abstract syntax tree.

    The function implements the grammar rule 19:
        expression_statement → expression “ ; ”
 ------------------------------------------------------------------------------ */
ASTNodePtr Parser::parseExpressionStatement() {
    ASTNodePtr node = parseExpression();
    match(SEMICOLON, "Expected ';' at the end of an expresison statement");
    return node;
}


/** ---------------------------------------------------------------------------
    Definition of the parseExpression function.
    The function will validate the syntax of an expression statement
    and build its abstract syntax tree.

    The function implements the grammar rule 7:
        expression → add_expression
 ------------------------------------------------------------------------------ */
ASTNodePtr Parser::parseExpression() {
    return parseAddExpression();
}


/** ---------------------------------------------------------------------------
    Definition of the parseAddExpression function.
    The function will validate the syntax of an expression statement for
    addition and subtraction and build its abstract syntax tree.

    The function implements the grammar rule 8:
        add_expression → mul_expression ( (“ + ” | “ - ”) mul_expression )*
 ------------------------------------------------------------------------------ */
ASTNodePtr Parser::parseAddExpression() {
    ASTNodePtr left = parseMulExpression();

    // Using left recursion removal method to ensure operator precedence.
    // (Louden & Lamber, 2011, p. 226)
    while (peek().type == PLUS || peek().type == MINUS) {
        Token op = match(peek().type, "Expected '+' or '-'");
        ASTNodePtr right = parseMulExpression();
        left = std::make_shared<BinaryOperationNode>(op.value, left, right);
    }

    return left;
}


/** ---------------------------------------------------------------------------
    Definition of the parseMulExpression function.
    The function will validate the syntax of an expression statement for
    multiplication and division and build its abstract syntax tree.

    The function implements the grammar rule 9:
        mul_expression → pow_expression ( (“ * ” | “ / ”) pow_expression )*
 ------------------------------------------------------------------------------ */
ASTNodePtr Parser::parseMulExpression() {
    ASTNodePtr left = parsePowExpression();
    while (peek().type == STAR || peek().type == SLASH)  {
        Token op = match(peek().type, "Expected '*' or '/'");
        ASTNodePtr right = parsePowExpression();
        left = std::make_shared<BinaryOperationNode>(op.value, left, right);
    }

    return left;
}


/** ---------------------------------------------------------------------------
    Definition of the parsePowExpression function.
    The function will validate the syntax of an expression statement for
    power and build its abstract syntax tree.

    The function implements the grammar rule 10:
        pow_expression → unary_expression (“ ^ ” pow_expression)?
 ------------------------------------------------------------------------------ */
ASTNodePtr Parser::parsePowExpression() {
    ASTNodePtr left = parseUnaryExpression();
    if (peek().type == CARET) {
        Token op = match(CARET, "Expected '^'");
        ASTNodePtr right = parsePowExpression();
        return std::make_shared<BinaryOperationNode>(op.value, left, right);
    }

    return left;
}


/** ---------------------------------------------------------------------------
    Definition of the parseUnaryExpression function.
    The function will validate the syntax of an expression statement for
    unary operation and build its abstract syntax tree.

    The function implements the grammar rule 11:
        unary_expression → (“ + ” | “ - ”) unary_expression | primary
 ------------------------------------------------------------------------------ */
ASTNodePtr Parser::parseUnaryExpression() {
    if (peek().type == PLUS || peek().type == MINUS) {
        Token op = match(peek().type, "Expected an unary operator '+' or '-'");
        return std::make_shared<UnaryOperationNode>(op.value, parseUnaryExpression());
    }

    return parsePrimary();
}


/** ---------------------------------------------------------------------------
    Definition of the parsePrimary function.
    The function will validate the syntax of a primary expression that has
    a value.

    The function implements the grammar rule 12:
        primary → number | identifier | function_call | “ ( ” expression “ ) ”
 ------------------------------------------------------------------------------ */
ASTNodePtr Parser::parsePrimary() {
    if (peek().type == NUMBER) {
        return parseNumber();
    }

    else if (peek().type == IDENTIFIER) {
        if (pos + 1 < tokens.size() && tokens[pos + 1].type == LPAREN)
            return parseFunctionCall();
        else
            return parseIdentifier();
    }

    else if (peek().type == LPAREN) {
        match(LPAREN, "Expected '(' in a primary");
        ASTNodePtr expr = parseExpression();
        match(RPAREN, "Expected ')' at the end of a primary");
        return expr;
    }

    throw InvalidSyntax("Expected a value or expression");
}


/** ---------------------------------------------------------------------------
    Definition of the parseNumber function.
    The function validates the syntax of a numeric literal and build its syntax
    tree.

    The function implements the grammar rule 13:
        number → digit+ (“ . ” digit+)?
 ------------------------------------------------------------------------------ */
ASTNodePtr Parser::parseNumber() {
    try {
        Token number = match(NUMBER, "Expected a number");
        return std::make_shared<NumberNode>(stod(number.value));
    }
    catch (const std::invalid_argument &e) {
        throw InvalidSyntax("Expected a number");
    }
}


/** ---------------------------------------------------------------------------
    Definition of the parseFunctionCall function.
    The function validates the syntax of a function call satement and build its
    syntax tree.

    The function implements the grammar rule 14:
        function_call → identifier “ ( ” arg_list “ ) ”
 ------------------------------------------------------------------------------ */
ASTNodePtr Parser::parseFunctionCall() {
    Token name = match(IDENTIFIER, "Expected a function name in a function call");
    match(LPAREN, "Expect a '(' in a function called before arguments");
    std::vector<ASTNodePtr> args = parseArgList();
    match(RPAREN, "Expected a ')' in a funciton call after arguments");

    return std::make_shared<FunctionCallNode>(name.value, args);
}


/** ---------------------------------------------------------------------------
    Definition of the parseArgList function.
    The function validates the syntax of an argument list for a function call
    and build it syntax tree.

    The function implements the grammar rule 15:
        arg_list → expression (“ , ” expression)*
 ------------------------------------------------------------------------------ */
std::vector<ASTNodePtr> Parser::parseArgList() {
    std::vector<ASTNodePtr> args;
    if (peek().type == RPAREN)
        throw InvalidSyntax("Function call with no arguments");

    args.push_back(parseExpression());
    while (peek().type == COMMA) {
        match(COMMA, "Expected ',' between arguments in a function call");
        args.push_back(parseExpression());
    }

    return args;
}

/** ---------------------------------------------------------------------------
    Definition of the parseIdentifier function.
    The function validates the syntax of an identifier and build it syntax tree

    The function implements the grammar rule 4:
        identifier → letter (letter | digit | “ _ ”)*
 ------------------------------------------------------------------------------ */
ASTNodePtr Parser::parseIdentifier() {
    Token name = match(IDENTIFIER, "Expected an identifier");
    return std::make_shared<IdentifierNode>(name.value);
}


/** ---------------------------------------------------------------------------
    Definition of the parse function.
    The function will check the syntax of the source code by inspecting tokens
    and builds their abstract syntax tree.
    @return a vector of ASTNodePtr
 ------------------------------------------------------------------------------ */
std::vector<ASTNodePtr> Parser::parse() {
    pos = 0;
    std::vector<ASTNodePtr> nodes;
    while (peek().type != EOF_TOKEN) {
        nodes.push_back(parseStatement());
    }

    return nodes;
}


/** ---------------------------------------------------------------------------
    Definition of the displayAST function.
    The function will display all abstract syntax trees of the program for
    debugging and demonstration purposes.
 ------------------------------------------------------------------------------ */
void Parser::displayAST() {
    try {
        pos = 0;
        while (peek().type != EOF_TOKEN) {
            ASTNodePtr node = parseStatement();
            node->print(0);
            std::cout << std::endl;
        }
        pos = 0;
    }
    catch (const InvalidSyntax &e) {
        pos = 0;
        std::cerr << e.what() << '\n';
    }
}