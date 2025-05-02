// include/scanner.h
#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>
#include "token.h"

class Scanner {
public:
    explicit Scanner(const std::string& source);
    std::vector<Token> scanTokens();

private:
    std::string source;
    size_t start = 0;
    size_t current = 0;
    int line = 1;

    bool isAtEnd();
    char advance();
    char peek();
    char peekNext();
    void addToken(TokenType type);
    void scanToken();
    void skipWhitespace();
    void identifier();
    void number();
    void stringLiteral();
    void singleLineComment();
    void multiLineComment();


    std::vector<Token> tokens;
};

#endif
