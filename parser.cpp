#include "parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens, SymbolTable& symtab)
    : tokens(tokens), symtab(symtab) {}

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

void Parser::error(const std::string& message) {
    std::cerr << "Parser Error at line " << peek().line << ": " << message << "\n";
    errorCount++;
}
void Parser::parseProgram() {
    std::cout << "\n--- Parser Output ---\n";
    while (!isAtEnd()) {

        // Handle comments first
        if (peek().type == TokenType::SingleComment ||
            peek().type == TokenType::SMultiComment ||
            peek().type == TokenType::CommentContent ||
            peek().type == TokenType::EMultiComment) {
            handleComment();
            continue;
        }
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
    }

        std::cerr << "\nTotal parser errors: " << errorCount << "\n";

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
        case TokenType::Void: varType = SymbolType::Void; break;
        default: varType = SymbolType::Unknown; break;
    }

    if (!match(TokenType::Identifier)) { error("Expected variable name"); return; }
    std::string varName = tokens[current - 1].lexeme;

    if (!symtab.declareVariable(varName, varType)) {
        std::cerr << "Error: Variable '" << varName << "' already declared (line " << tokens[current - 1].line << ")\n";
    }

    while (peek().type == TokenType::Comma) {
        advance();  // consume comma
        if (!match(TokenType::Identifier)) { error("Expected variable name"); return; }
        varName = tokens[current - 1].lexeme;
        if (!symtab.declareVariable(varName, varType)) {
            std::cerr << "Error: Variable '" << varName << "' already declared (line " << tokens[current - 1].line << ")\n";
        }
    }

    if (!match(TokenType::Semicolon)) { error("Expected ';'"); return; }

    std::cout << "✅ Matched: Declaration\n";
}

void Parser::functionDefinition() {
    advance(); // return type
    if (!match(TokenType::Identifier)) { error("Expected function name"); return; }

    if (!match(TokenType::LeftParen)) { error("Expected '('"); return; }

    // Skip params safely
    while (peek().type != TokenType::RightParen && !isAtEnd()) {
        advance();
    }
    if (!match(TokenType::RightParen)) { error("Expected ')'"); return; }

    std::cout << "✅ Matched: Function Definition\n";

    block();
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
    } else if (peek().type == TokenType::Semicolon) {
        match(TokenType::Semicolon);
        std::cout << "✅ Matched: Empty Statement\n";
    } else {
        expressionStatement();
    }
}

void Parser::expressionStatement() {
    expression();
    if (!match(TokenType::Semicolon)) { error("Expected ';'"); return; }
    std::cout << "✅ Matched: Expression Statement\n";
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

    std::cout << "✅ Matched: If/Else Statement\n";
}

void Parser::iterationStatement() {
    advance(); // RepeatWhen
    if (!match(TokenType::LeftParen)) { error("Expected '('"); return; }
    expression();
    if (!match(TokenType::RightParen)) { error("Expected ')'"); return; }

    statement();

    std::cout << "✅ Matched: Loop Statement\n";
}

void Parser::jumpStatement() {
    Token jumpTok = advance(); // Turnback or OutLoop

    if (jumpTok.type == TokenType::Return) {
        expression();
        if (!match(TokenType::Semicolon)) { error("Expected ';'"); return; }
        std::cout << "✅ Matched: Return Statement\n";
    } else if (jumpTok.type == TokenType::Break) {
        if (!match(TokenType::Semicolon)) { error("Expected ';'"); return; }
        std::cout << "✅ Matched: Break Statement\n";
    }
}

void Parser::assignment() {
    if (!match(TokenType::Identifier)) { error("Expected identifier"); return; }
    std::string varName = tokens[current - 1].lexeme;

    if (!symtab.exists(varName)) {
        std::cerr << "❌ Error: Variable '" << varName << "' not declared before use (line " << tokens[current - 1].line << ")\n";
    }

    if (!match(TokenType::Assignment)) { error("Expected '='"); return; }

    expression();

    if (!match(TokenType::Semicolon)) { error("Expected ';'"); return; }

    std::cout << "✅ Matched: Assignment\n";
}

void Parser::expression() {
    simpleExpression();
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
void Parser::handleComment() {
    if (match(TokenType::SingleComment)) {
        // Single-line comment
        if (match(TokenType::CommentContent)) {
            std::cout << "✅ Matched: Single-line comment: " << tokens[current-1].lexeme << "\n";
        }
    }
    else if (match(TokenType::SMultiComment)) {
        // Multi-line comment start
        while (!isAtEnd() && peek().type != TokenType::EMultiComment) {
            if (match(TokenType::CommentContent)) {
                std::cout << "✅ Matched: Multi-line comment part: " << tokens[current-1].lexeme << "\n";
            } else {
                advance(); // skip other tokens within the comment
            }
        }
        if (match(TokenType::EMultiComment)) {
            std::cout << "✅ Matched: Multi-line comment end\n";
        }
    }
    else {
        advance(); // skip any other comment-related tokens
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
        if (!match(TokenType::RightParen)) { error("Expected ')'"); }
    } else if (match(TokenType::Identifier)) {
        // Optional semantic check can be added here
    } else if (match(TokenType::IntgerConstant) || match(TokenType::FloatConstant)) {
        // constant is ok
    } else {
        error("Expected expression factor");
    }
}

void Parser::block() {
    if (!match(TokenType::LeftBrace)) { error("Expected '{'"); return; }

    while (peek().type != TokenType::RightBrace && !isAtEnd()) {
        statement();
    }

    if (!match(TokenType::RightBrace)) { error("Expected '}'"); return; }

    std::cout << "✅ Matched: Block\n";
}


