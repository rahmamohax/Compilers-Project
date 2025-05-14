// include/scanner.h
#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>
#include "token.h"

struct ScannerError {
    int line;
    std::string message;
    ScannerError(int line, const std::string& message) : line(line), message(message) {}
};

class Scanner {
public:
    explicit Scanner(const std::string& source);
    std::vector<Token> scanTokens();
    int getErrorCount() const { return errorCount; }
    const std::vector<ScannerError>& getErrors() const { return errors; }


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
    void invalidIdentifier(char firstChar);
    void number(char firstchar);
    void stringLiteral();
    void singleLineComment();
    void multiLineComment();
    int errorCount = 0;
    void error(const string& message);

    std::vector<Token> tokens;
    std::vector<ScannerError> errors;
};

#endif
