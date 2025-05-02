#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED



#include <vector>
#include "token.h"
#include "symbol_table.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens, SymbolTable& symtab);
    void parseProgram();
 int getErrorCount() const { return errorCount; }
private:
    const std::vector<Token>& tokens;
    SymbolTable& symtab;
    size_t current = 0;

    bool isAtEnd();
    const Token& advance();
    const Token& peek();
    bool match(TokenType type);
    void error(const string& message);

    void declaration();
    void functionDefinition();
    void statement();
    void expressionStatement();
    void selectionStatement();
    void iterationStatement();
    void jumpStatement();
    void assignment();
    void expression();
    void simpleExpression();
    void additiveExpression();
    void term();
    void factor();
    void block();
    void handleComment();
     int errorCount = 0;
};






#endif // PARSER_H_INCLUDED
