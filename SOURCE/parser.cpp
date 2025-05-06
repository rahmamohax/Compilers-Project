#include "parser.h"
#include <iostream>
#include <stdexcept>
using namespace std;

Parser::Parser(Scanner& s) : scanner(s), errorCount(0) {
    currentToken = scanner.getNextToken();
}

void Parser::advance() {
    currentToken = scanner.getNextToken();
}

void Parser::match(TokenType expected) {
    if (currentToken.type == expected) {
        advance();
    } else {
        reportError("Expected " + std::to_string(static_cast<int>(expected)) +
                   " but found " + std::to_string(static_cast<int>(currentToken.type)));
    }
}

void Parser::reportError(const std::string& message) {
 cout << "Line : " << currentToken.line << " Not Matched                     Error: " << message << "\n";
    errorCount++;
}

void Parser::reportMatch(const std::string& rule) {
 cout << "Line : " << currentToken.line << " Matched                           Rule used: " << rule << "\n";
}

std::unique_ptr<ASTNode> Parser::parse() {
 cout << "\nParser Phase Output:\n";
    auto program = parseProgram();
    if (currentToken.type != TokenType::END_OF_FILE) {
        reportError("Expected end of file");
    }
    
    if (errorCount > 0) {
     cout << "\nTotal NO of errors: " << errorCount << "\n";
    }
    return program;
}

std::unique_ptr<ASTNode> Parser::parseProgram() {
    auto program = std::make_unique<BlockNode>(currentToken.line, currentToken.column);
    
    while (currentToken.type != TokenType::END_OF_FILE) {
        if (currentToken.type == TokenType::SINGLE_COMMENT_START ||
            currentToken.type == TokenType::COMMENT_CONTENT ||
            currentToken.type == TokenType::MULTI_COMMENT_END) {
            reportMatch("Comment");
            advance();
        } else if (currentToken.type == TokenType::NORETURN || isTypeToken(currentToken.type)) {
            auto funcDecl = parseFunctionDecl();
            if (funcDecl) {
                reportMatch("fun-declaration");
                program->statements.push_back(std::move(funcDecl));
            }
        } else {
            reportError("Expected function declaration");
            advance();
        }
    }
    
    return program;
}

std::unique_ptr<ASTNode> Parser::parseFunctionDecl() {
    bool isNOReturn = (currentToken.type == TokenType::NORETURN);
    if (isNOReturn) {
        advance();
    } else {
        if (!isTypeToken(currentToken.type)) {
            reportError("Expected return type or NORETURN");
            return nullptr;
        }
    }
    
    TokenType returnType = isNOReturn ? TokenType::VOID : currentToken.type;
    if (!isNOReturn) {
        advance();
    }
    
    if (currentToken.type != TokenType::IDENTIFIER) {
        reportError("Expected function name");
        return nullptr;
    }
    
 string name = currentToken.value;
    advance();
    
    match(TokenType::LEFT_PAREN);
    
 vector<std::pair<std::string, TokenType>> parameters;
    while (currentToken.type != TokenType::RIGHT_PAREN) {
        if (!isTypeToken(currentToken.type)) {
            reportError("Expected parameter type");
            break;
        }
        
        TokenType paramType = currentToken.type;
        advance();
        
        if (currentToken.type != TokenType::IDENTIFIER) {
            reportError("Expected parameter name");
            break;
        }
        
        parameters.emplace_back(currentToken.value, paramType);
        advance();
        
        if (currentToken.type == TokenType::COMMA) {
            advance();
        } else if (currentToken.type != TokenType::RIGHT_PAREN) {
            reportError("Expected ',' or ')'");
            break;
        }
    }
    
    match(TokenType::RIGHT_PAREN);
    match(TokenType::LEFT_BRACE);
    
    auto body = parseBlock();
    
    match(TokenType::RIGHT_BRACE);
    
    if (isNOReturn) {
        return std::make_unique<NOReturnFuncNode>(name, std::move(parameters),
                                             move(body), currentToken.line,
                                                currentToken.column);
    }
    
    return std::make_unique<FunctionDeclNode>(name, returnType, std::move(parameters),
                                         move(body), currentToken.line,
                                            currentToken.column);
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    switch (currentToken.type) {
        case TokenType::IF_TRUE:
            return parseIfStmt();
        case TokenType::WHILE:
            return parseWhileStmt();
        case TokenType::REPEATWHEN:
            return parseRepeatWhenStmt();
        case TokenType::FOR:
            return parseForStmt();
        case TokenType::RETURN:
            return parseReturnStmt();
        case TokenType::LEFT_BRACE:
            return parseBlock();
        case TokenType::BREAK:
        case TokenType::CONTINUE: {
            auto stmt = std::make_unique<ASTNode>(
                currentToken.type == TokenType::BREAK ? NodeType::BREAK_STMT : NodeType::CONTINUE_STMT,
                currentToken.line, currentToken.column
            );
            advance();
            match(TokenType::SEMICOLON);
            return stmt;
        }
        default:
            if (isTypeToken(currentToken.type)) {
                return parseVariableDecl();
            }
            auto expr = parseExpression();
            match(TokenType::SEMICOLON);
            return expr;
    }
}

std::unique_ptr<ASTNode> Parser::parseBlock() {
    auto block = std::make_unique<BlockNode>(currentToken.line, currentToken.column);
    
    while (currentToken.type != TokenType::RIGHT_BRACE &&
           currentToken.type != TokenType::END_OF_FILE) {
        block->statements.push_back(parseStatement());
    }
    
    return block;
}

std::unique_ptr<ASTNode> Parser::parseIfStmt() {
    match(TokenType::IF_TRUE);
    match(TokenType::LEFT_PAREN);
    
    auto condition = parseExpression();
    
    match(TokenType::RIGHT_PAREN);
    match(TokenType::LEFT_BRACE);
    
    auto thenBranch = parseBlock();
    
    match(TokenType::RIGHT_BRACE);
    
 unique_ptr<ASTNode> elseBranch = nullptr;
    if (currentToken.type == TokenType::OTHERWISE) {
        advance();
        match(TokenType::LEFT_BRACE);
        elseBranch = parseBlock();
        match(TokenType::RIGHT_BRACE);
    }
    
    return std::make_unique<IfStmtNode>(std::move(condition), std::move(thenBranch),
                                   move(elseBranch), currentToken.line,
                                      currentToken.column);
}

std::unique_ptr<ASTNode> Parser::parseWhileStmt() {
    match(TokenType::WHILE);
    match(TokenType::LEFT_PAREN);
    
    auto condition = parseExpression();
    
    match(TokenType::RIGHT_PAREN);
    match(TokenType::LEFT_BRACE);
    
    auto body = parseBlock();
    
    match(TokenType::RIGHT_BRACE);
    
    return std::make_unique<WhileStmtNode>(std::move(condition), std::move(body),
                                         currentToken.line, currentToken.column);
}

std::unique_ptr<ASTNode> Parser::parseForStmt() {
    match(TokenType::FOR);
    match(TokenType::LEFT_PAREN);
    
 unique_ptr<ASTNode> initializer = nullptr;
    if (currentToken.type != TokenType::SEMICOLON) {
        if (isTypeToken(currentToken.type)) {
            initializer = parseVariableDecl();
        } else {
            initializer = parseExpression();
            match(TokenType::SEMICOLON);
        }
    } else {
        advance();
    }
    
 unique_ptr<ExpressionNode> condition = nullptr;
    if (currentToken.type != TokenType::SEMICOLON) {
        condition = parseExpression();
    }
    match(TokenType::SEMICOLON);
    
 unique_ptr<ExpressionNode> increment = nullptr;
    if (currentToken.type != TokenType::RIGHT_PAREN) {
        increment = parseExpression();
    }
    match(TokenType::RIGHT_PAREN);
    
    match(TokenType::LEFT_BRACE);
    auto body = parseBlock();
    match(TokenType::RIGHT_BRACE);
    
    return std::make_unique<ForStmtNode>(std::move(initializer), std::move(condition),
                                    move(increment), std::move(body),
                                       currentToken.line, currentToken.column);
}

std::unique_ptr<ASTNode> Parser::parseReturnStmt() {
    match(TokenType::RETURN);
    
 unique_ptr<ExpressionNode> value = nullptr;
    if (currentToken.type != TokenType::SEMICOLON) {
        value = parseExpression();
    }
    
    match(TokenType::SEMICOLON);
    
    return std::make_unique<ReturnStmtNode>(std::move(value), currentToken.line,
                                          currentToken.column);
}

std::unique_ptr<ASTNode> Parser::parseVariableDecl() {
    TokenType varType = currentToken.type;
    advance();
    
    if (currentToken.type != TokenType::IDENTIFIER) {
        reportError("Expected variable name");
        return nullptr;
    }
    
 string name = currentToken.value;
    advance();
    
 unique_ptr<ExpressionNode> initializer = nullptr;
    if (currentToken.type == TokenType::ASSIGN) {
        advance();
        initializer = parseExpression();
    }
    
    match(TokenType::SEMICOLON);
    
    return std::make_unique<VariableDeclNode>(name, varType, std::move(initializer),
                                            currentToken.line, currentToken.column);
}

std::unique_ptr<ExpressionNode> Parser::parseExpression() {
    return parseAssignment();
}

std::unique_ptr<ExpressionNode> Parser::parseAssignment() {
    auto expr = parseEquality();
    
    if (currentToken.type == TokenType::ASSIGN) {
        TokenType op = currentToken.type;
        advance();
        auto right = parseAssignment();
        return std::make_unique<BinaryExprNode>(op, std::move(expr), std::move(right),
                                              currentToken.line, currentToken.column);
    }
    
    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parseEquality() {
    auto expr = parseComparison();
    
    while (currentToken.type == TokenType::EQUAL ||
           currentToken.type == TokenType::NOT_EQUAL) {
        TokenType op = currentToken.type;
        advance();
        auto right = parseComparison();
        expr = std::make_unique<BinaryExprNode>(op, std::move(expr), std::move(right),
                                              currentToken.line, currentToken.column);
    }
    
    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parseComparison() {
    auto expr = parseTerm();
    
    while (currentToken.type == TokenType::LESS ||
           currentToken.type == TokenType::LESS_EQUAL ||
           currentToken.type == TokenType::GREATER ||
           currentToken.type == TokenType::GREATER_EQUAL) {
        TokenType op = currentToken.type;
        advance();
        auto right = parseTerm();
        expr = std::make_unique<BinaryExprNode>(op, std::move(expr), std::move(right),
                                              currentToken.line, currentToken.column);
    }
    
    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parseTerm() {
    auto expr = parseFactor();
    
    while (currentToken.type == TokenType::PLUS ||
           currentToken.type == TokenType::MINUS) {
        TokenType op = currentToken.type;
        advance();
        auto right = parseFactor();
        expr = std::make_unique<BinaryExprNode>(op, std::move(expr), std::move(right),
                                              currentToken.line, currentToken.column);
    }
    
    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parseFactor() {
    auto expr = parseUnary();
    
    while (currentToken.type == TokenType::MULTIPLY ||
           currentToken.type == TokenType::DIVIDE) {
        TokenType op = currentToken.type;
        advance();
        auto right = parseUnary();
        expr = std::make_unique<BinaryExprNode>(op, std::move(expr), std::move(right),
                                              currentToken.line, currentToken.column);
    }
    
    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parseUnary() {
    if (isUnaryOperator(currentToken.type)) {
        TokenType op = currentToken.type;
        advance();
        auto expr = parseUnary();
        return std::make_unique<UnaryExprNode>(op, std::move(expr),
                                             currentToken.line, currentToken.column);
    }
    
    return parsePrimary();
}

std::unique_ptr<ExpressionNode> Parser::parsePrimary() {
    switch (currentToken.type) {
        case TokenType::INTEGER_LITERAL:
        case TokenType::FLOAT_LITERAL:
        case TokenType::STRING_LITERAL:
        case TokenType::BOOL_LITERAL: {
            auto node = std::make_unique<LiteralNode>(currentToken.value,
                                                    currentToken.type,
                                                    currentToken.line,
                                                    currentToken.column);
            advance();
            return node;
        }
        case TokenType::IDENTIFIER: {
            auto node = std::make_unique<IdentifierNode>(currentToken.value,
                                                       currentToken.line,
                                                       currentToken.column);
            advance();
            return node;
        }
        case TokenType::LEFT_PAREN: {
            advance();
            auto expr = parseExpression();
            match(TokenType::RIGHT_PAREN);
            return expr;
        }
        default:
            reportError("Expected expression");
            return nullptr;
    }
}

bool Parser::isTypeToken(TokenType type) const {
    return type == TokenType::IMW || type == TokenType::FLOAT ||
           type == TokenType::STRING || type == TokenType::BOOL;
}

bool Parser::isBinaryOperator(TokenType type) const {
    return type == TokenType::PLUS || type == TokenType::MINUS ||
           type == TokenType::MULTIPLY || type == TokenType::DIVIDE ||
           type == TokenType::EQUAL || type == TokenType::NOT_EQUAL ||
           type == TokenType::LESS || type == TokenType::LESS_EQUAL ||
           type == TokenType::GREATER || type == TokenType::GREATER_EQUAL ||
           type == TokenType::AND || type == TokenType::OR;
}

bool Parser::isUnaryOperator(TokenType type) const {
    return type == TokenType::MINUS || type == TokenType::NOT;
}

int Parser::getOperatorPrecedence(TokenType type) const {
    switch (type) {
        case TokenType::MULTIPLY:
        case TokenType::DIVIDE:
            return 3;
        case TokenType::PLUS:
        case TokenType::MINUS:
            return 2;
        case TokenType::EQUAL:
        case TokenType::NOT_EQUAL:
        case TokenType::LESS:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER:
        case TokenType::GREATER_EQUAL:
            return 1;
        default:
            return 0;
    }
}

bool Parser::hasError() const {
    return errorCount > 0;
}

int Parser::getErrorCount() const {
    return errorCount;
}

std::unique_ptr<ASTNode> Parser::parseRepeatWhenStmt() {
    match(TokenType::REPEATWHEN);
    match(TokenType::LEFT_PAREN);
    
    auto condition = parseExpression();
    
    match(TokenType::RIGHT_PAREN);
    match(TokenType::LEFT_BRACE);
    
    auto body = parseBlock();
    
    match(TokenType::RIGHT_BRACE);
    
    return std::make_unique<RepeatWhenStmtNode>(std::move(condition), std::move(body),
                                              currentToken.line, currentToken.column);
}