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
    cerr << "Parser Error at line " << peek().line << ": " << message << "\n";
    errorCount++;
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (peek().type == TokenType::Semicolon) {
            advance();
            return;
        }
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

bool Parser::checkTypeCompatibility(SymbolType varType, const Token& valueToken) {
    switch (varType) {
        case SymbolType::Integer:
        case SymbolType::SInteger:
            return valueToken.type == TokenType::IntgerConstant;
        case SymbolType::Float:
        case SymbolType::SFloat:
            return valueToken.type == TokenType::FloatConstant;
        case SymbolType::Character:
            return valueToken.type == TokenType::CharConstant;
        case SymbolType::String:
            return valueToken.type == TokenType::StringConstant;
        default:
            return false;
    }
}

void Parser::parseProgram() {
    cout << "\n--- Parser Output ---\n";
    const size_t maxIterations = tokens.size() * 2;
    size_t iterations = 0;

    while (!isAtEnd() && iterations++ < maxIterations) {
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
                peek().type == TokenType::Float || peek().type == TokenType::SFloat ||
                peek().type == TokenType::Void) {
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

    switch (typeToken.type) {
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

    do {
        if (!match(TokenType::Identifier)) { error("Expected variable name"); return; }
        string varName = tokens[current - 1].lexeme;

        if (!symtab.declareVariable(varName, varType)) {
            cerr << "Error: Variable '" << varName << "' already declared (line " << tokens[current - 1].line << ")\n";
        }

        if (match(TokenType::Assignment)) {
            if (peek().type == TokenType::IntgerConstant || peek().type == TokenType::FloatConstant ||
                peek().type == TokenType::CharConstant || peek().type == TokenType::StringConstant) {
                Token valueToken = advance();
                if (!checkTypeCompatibility(varType, valueToken)) {
                    error("Type mismatch: Cannot assign " + valueToken.lexeme + " to variable of type " + symtab.typeToString(varType));
                }
            } else {
                expression();
                cout << "Warning: Type checking for complex expressions not fully implemented (line " << peek().line << ")\n";
            }
        }
    } while (match(TokenType::Comma));

    if (!match(TokenType::Semicolon)) { error("Expected ';'"); return; }

    cout << "Matched: var-declaration    Line::  " << peek().line - 1 << "\n";
}

void Parser::functionDefinition() {
    symtab.enterScope(); // Enter function scope

    Token returnType = advance();
    SymbolType returnSymType;

    switch (returnType.type) {
        case TokenType::Integer: returnSymType = SymbolType::Integer; break;
        case TokenType::SInteger: returnSymType = SymbolType::SInteger; break;
        case TokenType::Character: returnSymType = SymbolType::Character; break;
        case TokenType::String: returnSymType = SymbolType::String; break;
        case TokenType::Float: returnSymType = SymbolType::Float; break;
        case TokenType::SFloat: returnSymType = SymbolType::SFloat; break;
        case TokenType::Void: returnSymType = SymbolType::Void; break;
        default:
            error("Invalid return type");
            symtab.exitScope();
            return;
    }

    if (!match(TokenType::Identifier)) {
        error("Expected function name");
        symtab.exitScope();
        return;
    }
    string funcName = tokens[current - 1].lexeme;

    if (!match(TokenType::LeftParen)) {
        error("Expected '(' after function name");
        symtab.exitScope();
        return;
    }

    vector<pair<string, SymbolType>> parameters;
    vector<SymbolType> paramTypes;
    while (!match(TokenType::RightParen)) {
        if (peek().type == TokenType::Integer || peek().type == TokenType::SInteger ||
            peek().type == TokenType::Character || peek().type == TokenType::String ||
            peek().type == TokenType::Float || peek().type == TokenType::SFloat) {
            Token paramType = advance();
            SymbolType paramSymType;
            switch (paramType.type) {
                case TokenType::Integer: paramSymType = SymbolType::Integer; break;
                case TokenType::SInteger: paramSymType = SymbolType::SInteger; break;
                case TokenType::Character: paramSymType = SymbolType::Character; break;
                case TokenType::String: paramSymType = SymbolType::String; break;
                case TokenType::Float: paramSymType = SymbolType::Float; break;
                case TokenType::SFloat: paramSymType = SymbolType::SFloat; break;
                default:
                    error("Invalid parameter type");
                    symtab.exitScope();
                    return;
            }
            if (!match(TokenType::Identifier)) {
                error("Expected parameter name");
                symtab.exitScope();
                return;
            }
            string paramName = tokens[current - 1].lexeme;
            parameters.emplace_back(paramName, paramSymType);
            paramTypes.push_back(paramSymType);
            symtab.declareVariable(paramName, paramSymType);
        }
        if (!match(TokenType::Comma) && peek().type != TokenType::RightParen) {
            error("Expected ',' or ')' in parameter list");
            symtab.exitScope();
            return;
        }
    }

    if (!symtab.declareFunction(funcName, returnSymType, paramTypes)) {
        error("Function '" + funcName + "' already declared");
        symtab.exitScope();
        return;
    }

    if (!match(TokenType::LeftBrace)) {
        error("Expected '{' at start of function body");
        symtab.exitScope();
        return;
    }

    while (!match(TokenType::RightBrace)) {
        statement();
        if (isAtEnd()) {
            error("Unterminated function body");
            symtab.exitScope();
            return;
        }
    }

    cout << "Matched: fun-declaration (" << funcName << ") Line::  " << peek().line - 1 << "\n";
    if (!parameters.empty()) {
        cout << "Parameters:\n";
        for (const auto& param : parameters) {
            cout << "  - " << param.first << " (" << symtab.typeToString(param.second) << ")\n";
        }
    }

    symtab.exitScope(); // Exit function scope
}

void Parser::statement() {
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
    } else if (peek().type == TokenType::Void) {
        functionDefinition();
    } else if (peek().type == TokenType::Semicolon) {
        match(TokenType::Semicolon);
        cout << "Matched: Empty Statement\n";
    } else if (peek().type == TokenType::Integer || peek().type == TokenType::SInteger ||
               peek().type == TokenType::Character || peek().type == TokenType::String ||
               peek().type == TokenType::Float || peek().type == TokenType::SFloat) {
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
}

void Parser::expressionStatement() {
    expression();
    if (!match(TokenType::Semicolon)) { error("Expected ';'"); return; }
    cout << "Matched: Expression Statement\n";
}

void Parser::selectionStatement() {
    advance();
    if (!match(TokenType::LeftParen)) { error("Expected '('"); return; }
    expression();
    if (!match(TokenType::RightParen)) { error("Expected ')'"); return; }

    statement();

    if (peek().type == TokenType::Condition) {
        advance();
        statement();
    }

    cout << "Matched: If/Else Statement    Line::  " << peek().line - 1 << "\n";
}

void Parser::iterationStatement() {
    Token loopToken = advance();
    if (!match(TokenType::LeftParen)) {
        error("Expected '(' after loop condition");
        return;
    }

    expression();
    if (!match(TokenType::RightParen)) {
        error("Expected ')' after loop condition");
        return;
    }

    statement();

    cout << "Matched: Iteration-Statement (" << loopToken.lexeme << ") Line::  " << peek().line - 1 << "\n";
}

void Parser::jumpStatement() {
    Token jumpTok = advance();
    if (jumpTok.type == TokenType::Return) {
        expression();
        if (!match(TokenType::Semicolon)) { error("Expected ';'"); return; }
        cout << "Matched: Jump-Statement\n";
    } else if (jumpTok.type == TokenType::Break) {
        if (!match(TokenType::Semicolon)) { error("Expected ';'"); return; }
        cout << "Matched: Jump-Statement\n";
    }
}

void Parser::assignment() {
    if (!match(TokenType::Identifier)) { error("Expected identifier"); return; }
    string varName = tokens[current - 1].lexeme;

    if (!symtab.exists(varName)) {
        cerr << "Error: Variable '" << varName << "' not declared before use (line " << tokens[current - 1].line << ")\n";
    }

    if (!match(TokenType::Assignment)) { error("Expected '='"); return; }

    if (peek().type == TokenType::IntgerConstant || peek().type == TokenType::FloatConstant ||
        peek().type == TokenType::CharConstant || peek().type == TokenType::StringConstant) {
        Token valueToken = advance();
        SymbolType varType = symtab.getVariableType(varName);
        if (!checkTypeCompatibility(varType, valueToken)) {
            error("Type mismatch: Cannot assign " + valueToken.lexeme + " to variable of type " + symtab.typeToString(varType));
        }
    } else {
        expression();
    }

    if (!match(TokenType::Semicolon)) { error("Expected ';'"); return; }

    cout << "Matched: Assignment    Line::  " << peek().line - 1 << "\n";
}

void Parser::expression() {
    logicalOrExpression();
}

void Parser::logicalOrExpression() {
    logicalAndExpression();
    while (match(TokenType::Or)) {
        logicalAndExpression();
        cout << "Matched: Logical OR expression Line::  " << peek().line - 1 << "\n";
    }
}

void Parser::logicalAndExpression() {
    simpleExpression();
    while (match(TokenType::And)) {
        simpleExpression();
        cout << "Matched: Logical And expression Line::  " << peek().line - 1 << "\n";
    }
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
        if (!symtab.exists(tokens[current - 1].lexeme)) {
            cerr << "Error: Undefined variable '" << tokens[current - 1].lexeme
                 << "' (line " << tokens[current - 1].line << ")\n";
        }
    } else if (match(TokenType::IntgerConstant) || match(TokenType::FloatConstant) ||
               match(TokenType::CharConstant) || match(TokenType::StringConstant)) {
        // constant is ok
    } else {
        error("Expected expression factor");
        throw runtime_error("Invalid factor");
    }
}

void Parser::handleComment() {
    if (match(TokenType::SingleComment)) {
        if (match(TokenType::CommentContent)) {
            cout << "Matched: Single-line comment: " << tokens[current - 1].lexeme << "\n";
        }
    } else if (match(TokenType::SMultiComment)) {
        while (!isAtEnd() && peek().type != TokenType::EMultiComment) {
            if (match(TokenType::CommentContent)) {
                cout << "Matched: Multi-line comment part: " << tokens[current - 1].lexeme << "\n";
            } else {
                advance();
            }
        }
        if (match(TokenType::EMultiComment)) {
            cout << "Matched: Multi-line comment end\n";
        }
    } else {
        advance();
    }
}

void Parser::block() {
    symtab.enterScope();
    if (!match(TokenType::LeftBrace)) { error("Expected '{'"); return; }

    while (peek().type != TokenType::RightBrace && !isAtEnd()) {
        statement();
    }

    if (!match(TokenType::RightBrace)) { error("Expected '}'"); return; }

    cout << "Matched: Block    Line::  " << peek().line - 1 << "\n";
    symtab.exitScope();
}
