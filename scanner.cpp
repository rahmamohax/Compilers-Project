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
    start= current;

    char c = advance();

    // Single character tokens
    switch (c) {
        case '+': addToken(TokenType::Plus); break;
        case '-':
            if (peek() == '>') { advance(); addToken(TokenType::Access); }
            else addToken(TokenType::Minus);
            break;
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
      //   case '"': stringLiteral(); break;
        case '/':
                if (peek() == '^') { // /^ Single-line comment
                    advance();
                    addToken(TokenType::SingleComment);
                    singleLineComment();  // handle content
                } else if (peek() == '@') { // /@ Start multi-line comment
                    advance();
                    addToken(TokenType::SMultiComment);
                    multiLineComment();  // handle content
                } else {
                    addToken(TokenType::Divide);
                }
    break;


        default:
            if (isalpha(c) || c == '_') {
                identifier();
            } else if (isdigit(c)) {
                number();
            } else {
                cerr << "Line " << line << ": Unexpected character '" << c << "'\n";
            }
            break;
    }
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

void Scanner::number() {
    bool isFloat = false;

    while (isdigit(peek())) advance();

    if (peek() == '.' && isdigit(peekNext())) {
        isFloat = true;
        advance(); // consume '.'
        while (isdigit(peek())) advance();
    }

    if (isFloat) {
        addToken(TokenType::FloatConstant);
    } else {
        addToken(TokenType::IntgerConstant);
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
    cerr << "Line " << line << ": Unterminated multi-line comment\n";
}
