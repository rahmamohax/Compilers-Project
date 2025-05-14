#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include <vector>
#include <string>
#include <unordered_set>
#include "token.h"
#include "symbol_table.h"

using std::string;

class Parser {
public:
    Parser(const std::vector<Token>& tokens, SymbolTable& symtab);
    void parseProgram();
    int getErrorCount() const { return errorCount; }

private:
    const std::vector<Token>& tokens;
    SymbolTable& symtab;
    size_t current = 0;
    int errorCount = 0;
    int lastErrorLine = -1; // Track the last line where an error occurred
    std::unordered_set<int> linesWithErrors; // Track all lines with errors

    bool isAtEnd();
    const Token& advance();
    const Token& peek();
    bool match(TokenType type);
    void error(const string& message);
    void synchronize();
    bool checkTypeCompatibility(SymbolType varType, const Token& valueToken);

    void declaration();
    void functionDefinition();
    void statement();
    void expressionStatement();
    void selectionStatement();
    void iterationStatement();
    void jumpStatement();
    void assignment();
    void expression();
    void logicalOrExpression();
    void logicalAndExpression();
    void simpleExpression();
    void additiveExpression();
    void term();
    void factor();
    void block();
    void handleComment();
};

#endif // PARSER_H_INCLUDED
