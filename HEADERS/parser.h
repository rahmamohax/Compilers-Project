#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include <memory>
#include <vector>
#include <string>

// AST Node Types
enum class NodeType {
    PROGRAM,
    FUNCTION_DECL,
    VARIABLE_DECL,
    BLOCK,
    IF_STMT,
    WHILE_STMT,
    FOR_STMT,
    RETURN_STMT,
    BREAK_STMT,
    CONTINUE_STMT,
    NORETURN_FUNC,
    REPEATWHEN_STMT,
    EXPRESSION,
    BINARY_EXPR,
    UNARY_EXPR,
    LITERAL,
    IDENTIFIER,
    CALL_EXPR,
    ASSIGN_EXPR
};

// AST Node Base Class
class ASTNode {
public:
    NodeType type;
    int line;
    int column;
    
    ASTNode(NodeType t, int l, int c) : type(t), line(l), column(c) {}
    virtual ~ASTNode() = default;
};

// Expression Node
class ExpressionNode : public ASTNode {
public:
    ExpressionNode(NodeType t, int l, int c) : ASTNode(t, l, c) {}
};

// Binary Expression Node
class BinaryExprNode : public ExpressionNode {
public:
    TokenType op;
    std::unique_ptr<ExpressionNode> left;
    std::unique_ptr<ExpressionNode> right;
    
    BinaryExprNode(TokenType o, std::unique_ptr<ExpressionNode> l, 
                  std::unique_ptr<ExpressionNode> r, int line, int col)
        : ExpressionNode(NodeType::BINARY_EXPR, line, col),
          op(o), left(std::move(l)), right(std::move(r)) {}
};

// Unary Expression Node
class UnaryExprNode : public ExpressionNode {
public:
    TokenType op;
    std::unique_ptr<ExpressionNode> expr;
    
    UnaryExprNode(TokenType o, std::unique_ptr<ExpressionNode> e, int line, int col)
        : ExpressionNode(NodeType::UNARY_EXPR, line, col),
          op(o), expr(std::move(e)) {}
};

// Literal Node
class LiteralNode : public ExpressionNode {
public:
    std::string value;
    TokenType literalType;
    
    LiteralNode(const std::string& v, TokenType t, int line, int col)
        : ExpressionNode(NodeType::LITERAL, line, col),
          value(v), literalType(t) {}
};

// Identifier Node
class IdentifierNode : public ExpressionNode {
public:
    std::string name;
    
    IdentifierNode(const std::string& n, int line, int col)
        : ExpressionNode(NodeType::IDENTIFIER, line, col),
          name(n) {}
};

// Function Declaration Node
class FunctionDeclNode : public ASTNode {
public:
    std::string name;
    TokenType returnType;
    std::vector<std::pair<std::string, TokenType>> parameters;
    std::unique_ptr<ASTNode> body;
    
    FunctionDeclNode(const std::string& n, TokenType rt, 
                    std::vector<std::pair<std::string, TokenType>> p,
                    std::unique_ptr<ASTNode> b, int line, int col)
        : ASTNode(NodeType::FUNCTION_DECL, line, col),
          name(n), returnType(rt), parameters(std::move(p)),
          body(std::move(b)) {}
};

// Variable Declaration Node
class VariableDeclNode : public ASTNode {
public:
    std::string name;
    TokenType varType;
    std::unique_ptr<ExpressionNode> initializer;
    
    VariableDeclNode(const std::string& n, TokenType t,
                    std::unique_ptr<ExpressionNode> i, int line, int col)
        : ASTNode(NodeType::VARIABLE_DECL, line, col),
          name(n), varType(t), initializer(std::move(i)) {}
};

// Block Node
class BlockNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    BlockNode(int line, int col)
        : ASTNode(NodeType::BLOCK, line, col) {}
};

// If Statement Node
class IfStmtNode : public ASTNode {
public:
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<ASTNode> thenBranch;
    std::unique_ptr<ASTNode> elseBranch;
    
    IfStmtNode(std::unique_ptr<ExpressionNode> c,
              std::unique_ptr<ASTNode> t,
              std::unique_ptr<ASTNode> e, int line, int col)
        : ASTNode(NodeType::IF_STMT, line, col),
          condition(std::move(c)), thenBranch(std::move(t)),
          elseBranch(std::move(e)) {}
};

// While Statement Node
class WhileStmtNode : public ASTNode {
public:
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<ASTNode> body;
    
    WhileStmtNode(std::unique_ptr<ExpressionNode> c,
                 std::unique_ptr<ASTNode> b, int line, int col)
        : ASTNode(NodeType::WHILE_STMT, line, col),
          condition(std::move(c)), body(std::move(b)) {}
};

// For Statement Node
class ForStmtNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> initializer;
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<ExpressionNode> increment;
    std::unique_ptr<ASTNode> body;
    
    ForStmtNode(std::unique_ptr<ASTNode> i,
               std::unique_ptr<ExpressionNode> c,
               std::unique_ptr<ExpressionNode> inc,
               std::unique_ptr<ASTNode> b, int line, int col)
        : ASTNode(NodeType::FOR_STMT, line, col),
          initializer(std::move(i)), condition(std::move(c)),
          increment(std::move(inc)), body(std::move(b)) {}
};

// Return Statement Node
class ReturnStmtNode : public ASTNode {
public:
    std::unique_ptr<ExpressionNode> value;
    
    ReturnStmtNode(std::unique_ptr<ExpressionNode> v, int line, int col)
        : ASTNode(NodeType::RETURN_STMT, line, col),
          value(std::move(v)) {}
};

// NOReturn Function Node
class NOReturnFuncNode : public ASTNode {
public:
    std::string name;
    std::vector<std::pair<std::string, TokenType>> parameters;
    std::unique_ptr<ASTNode> body;
    
    NOReturnFuncNode(const std::string& n,
                    std::vector<std::pair<std::string, TokenType>> p,
                    std::unique_ptr<ASTNode> b, int line, int col)
        : ASTNode(NodeType::NORETURN_FUNC, line, col),
          name(n), parameters(std::move(p)), body(std::move(b)) {}
};

// RepeatWhen Statement Node
class RepeatWhenStmtNode : public ASTNode {
public:
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<ASTNode> body;
    
    RepeatWhenStmtNode(std::unique_ptr<ExpressionNode> c,
                      std::unique_ptr<ASTNode> b, int line, int col)
        : ASTNode(NodeType::REPEATWHEN_STMT, line, col),
          condition(std::move(c)), body(std::move(b)) {}
};

// Parser Class
class Parser {
public:
    Parser(Scanner& scanner);
    std::unique_ptr<ASTNode> parse();
    bool hasError() const;
    int getErrorCount() const;
    
private:
    Scanner& scanner;
    Token currentToken;
    int errorCount;
    
    void advance();
    void match(TokenType expected);
    void reportError(const std::string& message);
    void reportMatch(const std::string& rule);
    
    // Parsing methods
    std::unique_ptr<ASTNode> parseProgram();
    std::unique_ptr<ASTNode> parseFunctionDecl();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseBlock();
    std::unique_ptr<ASTNode> parseIfStmt();
    std::unique_ptr<ASTNode> parseWhileStmt();
    std::unique_ptr<ASTNode> parseForStmt();
    std::unique_ptr<ASTNode> parseReturnStmt();
    std::unique_ptr<ASTNode> parseRepeatWhenStmt();
    std::unique_ptr<ASTNode> parseVariableDecl();
    std::unique_ptr<ExpressionNode> parseExpression();
    std::unique_ptr<ExpressionNode> parseAssignment();
    std::unique_ptr<ExpressionNode> parseEquality();
    std::unique_ptr<ExpressionNode> parseComparison();
    std::unique_ptr<ExpressionNode> parseTerm();
    std::unique_ptr<ExpressionNode> parseFactor();
    std::unique_ptr<ExpressionNode> parseUnary();
    std::unique_ptr<ExpressionNode> parsePrimary();
    
    // Helper methods
    bool isTypeToken(TokenType type) const;
    bool isBinaryOperator(TokenType type) const;
    bool isUnaryOperator(TokenType type) const;
    int getOperatorPrecedence(TokenType type) const;
};

#endif // PARSER_H 