#include "parser.h"
#include <iostream>
#include <stdexcept>
 using namespace std;


Parser::Parser(const vector<Token>& tokens, SymbolTable& symtab)
    : tokens(tokens), symtab(symtab), current(0), errorCount(0) {}

bool Parser::isAtEnd() {
    return current >= tokens.size() || tokens[current].type == TokenType::EndOfFile;
}

const Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

const Token& Parser::peek() {
    return tokens[current];
}

bool Parser::match(TokenType type) {
    if (!isAtEnd() && peek().type == type) {
        advance();
        return true;
    }
    return false;
}

void Parser::error(const string& message) {
    cerr << "Parser Error at line   " <<"Line:: "<<peek().line << ": " << message << "\n";
    errorCount++;
}

void Parser::synchronize() {
    advance(); // Skip the problematic token

    // Skip until we find a statement boundary
    while (!isAtEnd()) {
        if (peek().type == TokenType::Semicolon) {
            advance();
            return;
        }

        // Statement-starting tokens
        if (peek().type == TokenType::Integer || peek().type == TokenType::SInteger ||
            peek().type == TokenType::Character || peek().type == TokenType::String ||
            peek().type == TokenType::Float || peek().type == TokenType::SFloat ||
            peek().type == TokenType::Void || peek().type == TokenType::Condition ||
            peek().type == TokenType::Loop || peek().type == TokenType::Return ||
            peek().type == TokenType::Break || peek().type == TokenType::LeftBrace) {
            return;
        }

        advance();
    }
}

void Parser::parseProgram() {
    cout << "\n--- Parser Output ---\n";
    const size_t maxIterations = tokens.size() * 2; // Safety limit
    size_t iterations = 0;

    while (!isAtEnd() && iterations++ < maxIterations) {
        // Handle comments first
        if (peek().type == TokenType::SingleComment ||
            peek().type == TokenType::SMultiComment ||
            peek().type == TokenType::CommentContent ||
            peek().type == TokenType::EMultiComment) {
            handleComment();
            continue;
        }

        try {
            if (peek().type == TokenType::Integer || peek().type == TokenType::SInteger ||
                peek().type == TokenType::Character || peek().type == TokenType::String ||
                peek().type == TokenType::Float || peek().type == TokenType::SFloat
                ) {
                if (current + 2 < tokens.size() &&
                    tokens[current + 1].type == TokenType::Identifier &&
                    tokens[current + 2].type == TokenType::LeftParen) {
                    functionDefinition();
                } else {
                    declaration();
                }
            } else {
                statement();
            }
        } catch (const exception& e) {
            error(string("Parsing error: ") + e.what());
            synchronize();
        }
    }

    if (iterations >= maxIterations) {
        error("Parser stuck in infinite loop - aborting");
    }

    cerr << "\nTotal parser errors: " << errorCount << "\n";
}

void Parser::declaration() {
    Token typeToken = advance();
    SymbolType varType;

    switch(typeToken.type) {
        case TokenType::Integer: varType = SymbolType::Integer; break;
        case TokenType::SInteger: varType = SymbolType::SInteger; break;
        case TokenType::Character: varType = SymbolType::Character; break;
        case TokenType::String: varType = SymbolType::String; break;
        case TokenType::Float: varType = SymbolType::Float; break;
        case TokenType::SFloat: varType = SymbolType::SFloat; break;

        default:
            error("Invalid type");
            return;
    }
    if (!match(TokenType::Identifier)) { error("Expected variable name"); return; }
    string varName = tokens[current - 1].lexeme;

    if (!symtab.declareVariable(varName, varType)) {
        cerr << "Error: Variable '" << varName << "' already declared (line " << tokens[current - 1].line << ")\n";
    }

    while (peek().type == TokenType::Comma) {
        advance();  // consume comma
        if (!match(TokenType::Identifier)) { error("Expected variable name"); return; }
        varName = tokens[current - 1].lexeme;
        if (!symtab.declareVariable(varName, varType)) {
            cerr << "Error: Variable '" << varName << "' already declared (line " << tokens[current - 1].line << ")\n";
        }
    }

    if (!match(TokenType::Semicolon)) { error("Expected ';'"); return; }

    cout << "Matched: var-declaration    "<<"Line::  "<<peek().line-1<<"\n";
}

void Parser::functionDefinition() {

    // Handle NOReturn (void) along with other types
    Token returnType = advance();
    SymbolType returnSymType;

    switch(returnType.type) {
        case TokenType::Integer: returnSymType = SymbolType::Integer; break;
        case TokenType::SInteger: returnSymType = SymbolType::SInteger; break;
        case TokenType::Character: returnSymType = SymbolType::Character; break;
        case TokenType::String: returnSymType = SymbolType::String; break;
        case TokenType::Float: returnSymType = SymbolType::Float; break;
        case TokenType::SFloat: returnSymType = SymbolType::SFloat; break;
        case TokenType::Void: returnSymType = SymbolType::Void; break;  // Added this line
        default:
            error("Invalid return type");
            return;
    }

    if (!match(TokenType::Identifier)) { error("Expected function name"); return; }
    string funcName = tokens[current-1].lexeme;

    if (!match(TokenType::LeftParen)) { error("Expected '(' after function name"); return; }
    // For now, skip parameters
    while (!match(TokenType::RightParen)) {
        advance();
    }


    if (!match(TokenType::LeftBrace)) { error("Expected '{' at start of function body"); return; }

    while (!match(TokenType::RightBrace)) {
        statement();
        if (isAtEnd()) { error("Unterminated function body"); return; }
    }

    cout << " Matched:  fun-declaration   (" << funcName << ") "<<"Line::  "<<peek().line-1<<"\n";
    // Rest of the function definition code...
}

void Parser::statement() {
    // Handle comments first
    if (peek().type == TokenType::SingleComment ||
        peek().type == TokenType::SMultiComment ||
        peek().type == TokenType::CommentContent ||
        peek().type == TokenType::EMultiComment) {
        handleComment();
        return;
    }

    if (peek().type == TokenType::Identifier) {
        assignment();
    } else if (peek().type == TokenType::Condition) {
        selectionStatement();
    } else if (peek().type == TokenType::Loop) {
        iterationStatement();
    } else if (peek().type == TokenType::Return || peek().type == TokenType::Break) {
        jumpStatement();
    } else if (peek().type == TokenType::LeftBrace) {
        block();
    }else if(peek().type==TokenType::Void){
        functionDefinition();
    }
     else if (peek().type == TokenType::Semicolon) {
        match(TokenType::Semicolon);
        cout << " Matched: Empty Statement\n";
    } else if (peek().type == TokenType::Integer || peek().type == TokenType::SInteger ||
                        peek().type == TokenType::Character || peek().type == TokenType::String ||
                        peek().type == TokenType::Float || peek().type == TokenType::SFloat
                        ) {
                        if (current + 2 < tokens.size() &&
                            tokens[current + 1].type == TokenType::Identifier &&
                            tokens[current + 2].type == TokenType::LeftParen) {
                            functionDefinition();
                        } else {
                            declaration();
                        }


   } else {
        expressionStatement();
    }

    //
}

void Parser::expressionStatement() {
    expression();
    if (!match(TokenType::Semicolon)) { error("Expected ';'"); return; }
    cout << " Matched: Expression Statement\n";
}

void Parser::selectionStatement() {
    advance(); // IfTrue
    if (!match(TokenType::LeftParen)) { error("Expected '('"); return; }
    expression();
    if (!match(TokenType::RightParen)) { error("Expected ')'"); return; }

    statement();

    if (peek().type == TokenType::Condition) { // Otherwise
        advance();
        statement();
    }

    cout << " Matched: If/Else Statement    "<<"Line::  "<<peek().line-1;
}

void Parser::iterationStatement() {
    Token loopToken = advance(); // Could be RepeatWhen or Reiterate

    if (!match(TokenType::LeftParen)) {
        error("Expected '(' after loop condition");
        return;
    }

    expression(); // Parse the condition

    if (!match(TokenType::RightParen)) {
        error("Expected ')' after loop condition");
        return;
    }

    statement(); // Parse the loop body

    cout << " Matched: Itration-Statement (" << loopToken.lexeme << ")  "<< peek().line-1<<"\n";
}

void Parser::jumpStatement() {
    Token jumpTok = advance(); // Turnback or OutLoop

    if (jumpTok.type == TokenType::Return) {
        expression();
        if (!match(TokenType::Semicolon)) { error("Expected ';'"); return; }
        cout << " Matched: Jump-Statement\n";
    } else if (jumpTok.type == TokenType::Break) {
        if (!match(TokenType::Semicolon)) { error("Expected ';'"); return; }
        cout << " Matched: Jump-Statement\n";
    }
}

void Parser::assignment() {
    if (!match(TokenType::Identifier)) { error("Expected identifier"); return; }
    string varName = tokens[current - 1].lexeme;

    if (!symtab.exists(varName)) {
        cerr << " Error: Variable '" << varName << "' not declared before use (line " << tokens[current - 1].line << ")\n";
    }

    if (!match(TokenType::Assignment)) { error("Expected '='"); return; }

    expression();

    if (!match(TokenType::Semicolon)) { error("Expected ';'"); return; }

    cout << " Matched: Assignment   "<<"Line::  "<<peek().line-1<<"\n";
}

void Parser::expression() {
    logicalOrExpression();
}

void Parser::logicalOrExpression() {
    logicalAndExpression();
    while (match(TokenType::Or)) {
        logicalAndExpression();
        cout << " Matched: Logical OR expression"<<"Line::  "<<peek().line-1<<"\n";
    }
}

void Parser::logicalAndExpression() {
    simpleExpression();
    while (match(TokenType::And)) {
        simpleExpression();
cout << " Matched: Logical And expression"<<"Line::  "<<peek().line-1<<"\n";    }
}

void Parser::simpleExpression() {
    additiveExpression();
    if (peek().type == TokenType::Less || peek().type == TokenType::Greater ||
        peek().type == TokenType::Equal || peek().type == TokenType::NotEqual ||
        peek().type == TokenType::LessEqual || peek().type == TokenType::GreaterEqual) {
        advance();
        additiveExpression();
    }
}

void Parser::additiveExpression() {
    term();
    while (peek().type == TokenType::Plus || peek().type == TokenType::Minus) {
        advance();
        term();
    }
}

void Parser::term() {
    factor();
    while (peek().type == TokenType::Multiply || peek().type == TokenType::Divide) {
        advance();
        factor();
    }
}

void Parser::factor() {
    if (match(TokenType::LeftParen)) {
        expression();
        if (!match(TokenType::RightParen)) {
            error("Expected ')'");
            throw runtime_error("Unmatched parenthesis");
        }
    } else if (match(TokenType::Identifier)) {
        if (!symtab.exists(tokens[current-1].lexeme)) {
            cerr << " Error: Undefined variable '" << tokens[current-1].lexeme
                     << "' (line " << tokens[current-1].line << ")\n";
        }
    } else if (match(TokenType::IntgerConstant) || match(TokenType::FloatConstant)) {
        // constant is ok
    } else {
        error("Expected expression factor");
        throw runtime_error("Invalid factor");
    }
}

void Parser::handleComment() {
    if (match(TokenType::SingleComment)) {
        // Single-line comment
        if (match(TokenType::CommentContent)) {
            cout << " Matched: Single-line comment: " << tokens[current-1].lexeme << "\n";
        }
    }
    else if (match(TokenType::SMultiComment)) {
        // Multi-line comment start
        while (!isAtEnd() && peek().type != TokenType::EMultiComment) {
            if (match(TokenType::CommentContent)) {
                cout << " Matched: Multi-line comment part: " << tokens[current-1].lexeme << "\n";
            } else {
                advance(); // skip other tokens within the comment
            }
        }
        if (match(TokenType::EMultiComment)) {
            cout << " Matched: Multi-line comment end\n";
        }
    }
    else {
        advance(); // skip any other comment-related tokens
    }
}

void Parser::block() {
    if (!match(TokenType::LeftBrace)) { error("Expected '{'"); return; }

    while (peek().type != TokenType::RightBrace && !isAtEnd()) {
        statement();
    }

    if (!match(TokenType::RightBrace)) { error("Expected '}'"); return; }

    cout << " Matched: Block    " << peek().line-1<<"\n";
}
