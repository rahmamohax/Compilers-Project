// src/scanner.cpp
#include"scanner.h"
#include "token.h"
#include <cctype>
#include <iostream>
#include <unordered_map>
using namespace std;

Scanner::Scanner(const string& source) : source(source) {}

vector<Token> Scanner::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.emplace_back(TokenType::EndOfFile, "", line);
    // Error reporting is now handled by the compiler
    return tokens;
}

bool Scanner::isAtEnd() {
    return current >= source.length();
}

char Scanner::advance() {
 return source[current++];
}
char Scanner::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Scanner::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void Scanner::addToken(TokenType type) {
    string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, line);
}

void Scanner::skipWhitespace() {
    while (true) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r') {
            advance();
        } else if (c == '\n') {
            line++;
            advance();
        } else {
            break;
        }
    }
}

void Scanner::scanToken() {
    skipWhitespace();
    if (isAtEnd()) return;
    start = current;

    char c = advance();

    // Handle + or - and check if followed by digit (signed number)
    if (c == '+' || c == '-') {
        if (isdigit(peek())) {
            number(c);  // pass + or - to number()
            return;
        } else {
            addToken((c == '+') ? TokenType::Plus : TokenType::Minus);
            return;
        }
    }

    // Single character tokens
    switch (c) {
        case '*': addToken(TokenType::Multiply); break;
        case '=':
            if (peek() == '=') { advance(); addToken(TokenType::Equal); }
            else addToken(TokenType::Assignment);
            break;
        case '!':
            if (peek() == '=') { advance(); addToken(TokenType::NotEqual); }
            break;
        case '<':
            if (peek() == '=') { advance(); addToken(TokenType::LessEqual); }
            else addToken(TokenType::Less);
            break;
        case '>':
            if (peek() == '=') { advance(); addToken(TokenType::GreaterEqual); }
            else addToken(TokenType::Greater);
            break;
        case '&':
            if (peek() == '&') { advance(); addToken(TokenType::And); }
            break;
        case '|':
            if (peek() == '|') { advance(); addToken(TokenType::Or); }
            break;
        case '~': addToken(TokenType::Not); break;
        case '{': addToken(TokenType::LeftBrace); break;
        case '}': addToken(TokenType::RightBrace); break;
        case '[': addToken(TokenType::LeftBracket); break;
        case ']': addToken(TokenType::RightBracket); break;
        case '(': addToken(TokenType::LeftParen); break;
        case ')': addToken(TokenType::RightParen); break;
        case ';': addToken(TokenType::Semicolon); break;
        case ',': addToken(TokenType::Comma); break;
        case '/':
            if (peek() == '^') { advance(); addToken(TokenType::SingleComment); singleLineComment(); }
            else if (peek() == '@') { advance(); addToken(TokenType::SMultiComment); multiLineComment(); }
            else { addToken(TokenType::Divide); }
            break;

        default:
            if (isalpha(c) || c == '_') {
                identifier();
            }
            else if (isdigit(c)) {
                // Check if this is an invalid identifier (digit followed by letters)
                if (isalpha(peek()) || peek() == '_') {
                    invalidIdentifier(c);
                } else {
                    number(c);
                }
            }
            else {
                error("Unexpected character '" + std::string(1, c) + "'");
            }
            break;
    }
}

void Scanner::invalidIdentifier(char firstChar) {
    // Consume all alphanumeric characters
    while (isalnum(peek()) || peek() == '_') advance();
    string text = source.substr(start, current - start);
    error("Invalid identifier '" + text + "' - identifiers cannot start with a digit");
    // Still add the token but mark it as Invalid
    tokens.emplace_back(TokenType::Invalid, text, line);
}

// Keyword map
unordered_map<string, TokenType> keywords = {
    {"IfTrue", TokenType::Condition},
    {"Otherwise", TokenType::Condition},
    {"Imw", TokenType::Integer},
    {"SIMw", TokenType::SInteger},
    {"Chj", TokenType::Character},
    {"Series", TokenType::String},
    {"IMwf", TokenType::Float},
    {"SIMwf", TokenType::SFloat},
    {"NOReturn", TokenType::Void},
    {"RepeatWhen", TokenType::Loop},
    {"Reiterate", TokenType::Loop},
    {"Turnback", TokenType::Return},
    {"OutLoop", TokenType::Break},
    {"Loli", TokenType::Struct},
    {"Include", TokenType::Include}
};

void Scanner::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();
    string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        addToken(it->second);
    } else {
        addToken(TokenType::Identifier);
    }
}

void Scanner::number(char firstChar) {
    bool isFloat = false;
    bool isSigned = false;
    size_t numberStart = start;

    if (firstChar == '+' || firstChar == '-') {
        isSigned = true;
        advance();  // consume '+' or '-'
    }

    while (isdigit(peek())) advance();

    if (peek() == '.' && isdigit(peekNext())) {
        isFloat = true;
        advance(); // consume '.'
        while (isdigit(peek())) advance();
    }

    // Extract full text (+4, -3.14, etc)
    string text = source.substr(numberStart, current - numberStart);

    if (isFloat) {
        if (isSigned)
            tokens.emplace_back(TokenType::SignedFloatConstant, text, line);
        else
            tokens.emplace_back(TokenType::FloatConstant, text, line);
    } else {
        if (isSigned)
            tokens.emplace_back(TokenType::SignedIntegerConstant, text, line);
        else
            tokens.emplace_back(TokenType::IntgerConstant, text, line);
    }
}


void Scanner::singleLineComment() {
    size_t commentStart = current;
    while (peek() != '\n' && !isAtEnd()) advance();
    string commentText = source.substr(commentStart, current - commentStart);
    tokens.emplace_back(TokenType::CommentContent, commentText, line);
}

void Scanner::multiLineComment() {
    size_t commentStart = current;
    while (!isAtEnd()) {
        if (peek() == '@' && peekNext() == '/') {
            string commentText = source.substr(commentStart, current - commentStart);
            tokens.emplace_back(TokenType::CommentContent, commentText, line);
            advance(); advance();  // consume @/
            addToken(TokenType::EMultiComment);
            return;
        }
        if (peek() == '\n') line++;
        advance();
    }

    // If reached here → unterminated comment
 error("Unterminated multi-line comment");
}

void Scanner::error(const std::string& message) {
    // Store error instead of printing immediately
    errors.emplace_back(line, message);
    errorCount++;
}
