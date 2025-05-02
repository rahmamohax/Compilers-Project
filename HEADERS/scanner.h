#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <memory>
#include <iostream>

// Token types
enum class TokenType {
    // Keywords
    NORETURN,
    REPEATWHEN,
    REG,
    IF_TRUE,
    OTHERWISE,
    IMW,
    FLOAT,
    STRING,
    BOOL,
    VOID,
    RETURN,
    WHILE,
    FOR,
    BREAK,
    CONTINUE,
    
    // Identifiers and literals
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    BOOL_LITERAL,
    
    // Operators
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    AND,
    OR,
    NOT,
    EQUAL,
    NOT_EQUAL,
    LESS,
    GREATER,
    LESS_EQUAL,
    GREATER_EQUAL,
    ASSIGN,
    
    // Delimiters
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    SEMICOLON,
    COMMA,
    
    // Comments
    SINGLE_COMMENT_START,  // /^
    MULTI_COMMENT_START,   // /@
    MULTI_COMMENT_END,     // @/
    COMMENT_CONTENT,       // Content between /@ and @/
    
    // Types
    TYPE,
    
    // Error handling
    ERROR,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token() : type(TokenType::ERROR), value(""), line(0), column(0) {}
    Token(TokenType t, const std::string& v, int l, int c)
        : type(t), value(v), line(l), column(c) {}
};

class Scanner {
public:
    Scanner();
    ~Scanner();
    
    bool openFile(const std::string& filename);
    Token getNextToken();
    bool hasError() const;
    int getErrorCount() const;
    
private:
    std::ifstream currentFile;
    std::string currentFilename;
    int currentLine;
    int currentColumn;
    int errorCount;
    
    std::unordered_map<std::string, TokenType> keywords;
    
    void initializeKeywords();
    Token scanToken();
    Token scanIdentifier();
    Token scanNumber();
    Token scanString();
    Token scanOperator();
    Token scanComment();
    void skipWhitespace();
    char peek(int offset = 0);
    char advance();
    bool isAtEnd();
    void handleInclude(const std::string& filename);
    
    // Error handling
    Token errorToken(const std::string& message);
    void reportError(const std::string& message);
};

#endif // SCANNER_H 