#include "../include/Parser.h"

/* Definition of the parseStatement function */
ASTNodePtr Parser::parseStatement()
{
    // Parse variable declaration statement or function declaration statement.
    if (peek().type == LET)
    {
        match(LET, "Expected 'let' in a declaration"); // Consume LET.
        if (peek().type == IDENTIFIER && pos + 1 < tokens.size() && tokens[pos + 1].type == LPAREN)
            return parseFunctionDeclaration();
        else
            return parseVariableDeclaration();
    }
    else if (peek().type == PRINT)
    {
        // Parser a printing statement.
        match(PRINT, "Expected 'print' in a print statement"); // Consume PRINT.
        return parsePrintStatement();
    }
    else if (peek().type == IDENTIFIER && pos + 1 < tokens.size() && tokens[pos + 1].type == EQUAL)
    {
        // Parse an assignment statement.
        return parseAssignment();
    }
    else return parseExpressionStatement();
    //else throw InvalidSyntax("Invalid statement in the source code");
}

/* Definition of the parseFunctionDeclaration function */
ASTNodePtr Parser::parseFunctionDeclaration()
{
    Token functionName = match(IDENTIFIER, "Expected a function name in a function declaration");
    match(LPAREN, "Expected '(' in a function declaration");
    Token parameter = match(IDENTIFIER, "Expect parameter in a function declaration");
    match(RPAREN, "Expected ')' in a function declaration");
    match(EQUAL, "Expected '=' in a function declaration");
    ASTNodePtr expr = parseExpression();
    match(SEMICOLON, "Expected ';' at the end of a function declaration");

    // Create a ASTNodePtr.
    return std::make_shared<FunctionDeclarationNode>(functionName.value, parameter.value, expr);
}

/* Definition of the parseVariableDeclaration function */
ASTNodePtr Parser::parseVariableDeclaration()
{
    Token varName = match(IDENTIFIER, "Expected an identifier after in a declaration");
    match(EQUAL, "Expect '=' after variable name in variable declaration");
    ASTNodePtr expr = parseExpression();
    match(SEMICOLON, "Expected ';' at the end of a statement");

    // Create a node of VariableDeclarationNode.
    return std::make_shared<VariableDeclarationNode>(varName.value, expr);
}

/* Definition of the parsePrintStatement function */
ASTNodePtr Parser::parsePrintStatement()
{
    match(LPAREN, "Expected '(' in a print statment");
    ASTNodePtr expr = parseExpression();
    match(RPAREN, "Expected a ')' in a print statement");
    match(SEMICOLON, "Expected ';' at the end of a print statement");

    return std::make_shared<PrintNode>(expr);
}

/* Definition of the parseAssignment function */
ASTNodePtr Parser::parseAssignment()
{
    Token identifier = match(IDENTIFIER, "Expected an identifier in an assignment");
    match(EQUAL, "Expected '=' after variable name in assignment");
    ASTNodePtr expr = parseExpression();
    match(SEMICOLON, "Expected ';' at the end of an assignment");

    return std::make_shared<AssignmentNode>(identifier.value, expr);
}

/* Definition of the parseExpressionStatement function */
ASTNodePtr Parser::parseExpressionStatement()
{
    ASTNodePtr node = parseExpression();
    match(SEMICOLON, "Expected ';' at the end of an expresison statement");
    return node;
}

/* Definition of the parseExpression function */
ASTNodePtr Parser::parseExpression()
{
    return parseAddExpression();
}

/* Definition of the parseExression function */
ASTNodePtr Parser::parseAddExpression()
{
    ASTNodePtr left = parseMulExpression();

    // Using left recursion removal method to ensure operator precedence.
    // (Louden & Lamber, 2011, p. 226)
    while (peek().type == PLUS || peek().type == MINUS)
    {
        Token op = match(peek().type, "Expected '+' or '-'");
        ASTNodePtr right = parseMulExpression();
        left = std::make_shared<BinaryOperationNode>(op.value, left, right);
    }

    return left;
}

/* Definition of the parseMulExpression function */
ASTNodePtr Parser::parseMulExpression()
{
    ASTNodePtr left = parsePowExpression();
    while (peek().type == STAR || peek().type == SLASH) 
    {
        Token op = match(peek().type, "Expected '*' or '/'");
        ASTNodePtr right = parsePowExpression();
        left = std::make_shared<BinaryOperationNode>(op.value, left, right);
    }

    return left;
}

/* Definition of the parsePowExpression function */
ASTNodePtr Parser::parsePowExpression()
{
    ASTNodePtr left = parseUnaryExpression();
    if (peek().type == CARET)
    {
        Token op = match(CARET, "Expected '^'");
        ASTNodePtr right = parsePowExpression();
        return std::make_shared<BinaryOperationNode>(op.value, left, right);
    }

    return left;
}

/* Definition of the parseUnaryExpression function */
ASTNodePtr Parser::parseUnaryExpression()
{
    if (peek().type == PLUS || peek().type == MINUS)
    {
        Token op = match(peek().type, "Expected an unary operator '+' or '-'");
        return std::make_shared<UnaryOperationNode>(op.value, parseUnaryExpression());
    }
    return parsePrimary();
}

/* Definition of the parsePrimary function */
ASTNodePtr Parser::parsePrimary()
{
    if (peek().type == NUMBER)
    {
        return parseNumber();
    }
    else if (peek().type == IDENTIFIER)
    {
        if (pos + 1 < tokens.size() && tokens[pos + 1].type == LPAREN)
            return parseFunctionCall();
        else
            return parseIdentifier();
    } else if (peek().type == LPAREN) {
        match(LPAREN, "Expected '(' in a primary");
        ASTNodePtr expr = parseExpression();
        match(RPAREN, "Expected ')' at the end of a primary");
        return expr;
    }
    throw InvalidSyntax("Expected a value or expression");
}

/* Definition of the parseNumber function */
ASTNodePtr Parser::parseNumber()
{
    try
    {
        Token number = match(NUMBER, "Expected a number");
        return std::make_shared<NumberNode>(stod(number.value));
    }
    catch (const std::invalid_argument &e)
    {
        throw InvalidSyntax("Expected a number");
    }
}

/* Definition of the parseFunctionCall function */
ASTNodePtr Parser::parseFunctionCall()
{
    Token name = match(IDENTIFIER, "Expected a function name in a function call");
    match(LPAREN, "Expect a '(' in a function called before arguments");
    std::vector<ASTNodePtr> args = parseArgList();
    match(RPAREN, "Expected a ')' in a funciton call after arguments");
    return std::make_shared<FunctionCallNode>(name.value, args);
}

/* Definition of the parseArgList function */
std::vector<ASTNodePtr> Parser::parseArgList()
{
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

/* Definition of the parseIndentifier function */
ASTNodePtr Parser::parseIdentifier()
{
    Token name = match(IDENTIFIER, "Expected an identifier");
    return std::make_shared<IdentifierNode>(name.value);
}

/* Definition of the parse function */
std::vector<ASTNodePtr> Parser::parse()
{
    pos = 0;
    std::vector<ASTNodePtr> nodes;
    while (peek().type != EOF_TOKEN)
    {
        nodes.push_back(parseStatement());
    }
    return nodes;
}

/* Definition of the displayAST function */
void Parser::displayAST()
{
    try
    {
        pos = 0;
        while (peek().type != EOF_TOKEN)
        {
            ASTNodePtr node = parseStatement();
            node->print(0);
            std::cout << std::endl;
        }
        pos = 0;
    }
    catch (const InvalidSyntax &e)
    {
        pos = 0;
        std::cerr << e.what() << '\n';
    }
}