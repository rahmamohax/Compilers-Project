#include "../HEADERS/scanner.h"
#include <cctype>
#include <sstream>
#include <fstream>
#include <string>

Scanner::Scanner() : currentLine(1), currentColumn(0), errorCount(0) {
    initializeKeywords();
}

Scanner::~Scanner() {
    if (currentFile.is_open()) {
        currentFile.close();
    }
}

void Scanner::initializeKeywords() {
    keywords = {
        {"NOReturn", TokenType::NORETURN},
        {"RepeatWhen", TokenType::REPEATWHEN},
        {"reg", TokenType::REG},
        {"IfTrue", TokenType::IF_TRUE},
        {"Otherwise", TokenType::OTHERWISE},
        {"Imw", TokenType::IMW},
        {"Float", TokenType::FLOAT},
        {"String", TokenType::STRING},
        {"Bool", TokenType::BOOL},
        {"Void", TokenType::VOID},
        {"Return", TokenType::RETURN},
        {"While", TokenType::WHILE},
        {"For", TokenType::FOR},
        {"Break", TokenType::BREAK},
        {"Continue", TokenType::CONTINUE}
    };
}

bool Scanner::openFile(const std::string& filename) {
    if (currentFile.is_open()) {
        currentFile.close();
    }
    
    currentFile.open(filename);
    if (!currentFile.is_open()) {
        reportError("Could not open file: " + filename);
        return false;
    }
    
    currentFilename = filename;
    currentLine = 1;
    currentColumn = 0;
    return true;
}

Token Scanner::getNextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return Token(TokenType::END_OF_FILE, "", currentLine, currentColumn);
    }
    
    char c = peek();
    Token token(TokenType::ERROR, "", currentLine, currentColumn);
    
    if (isalpha(c) || c == '_') {
        token = scanIdentifier();
    } else if (isdigit(c)) {
        token = scanNumber();
    } else if (c == '"' || c == '\'') {
        token = scanString();
    } else if (c == '/' && peek(1) == '^') {
        // Single-line comment
        advance(); // Skip '/'
        advance(); // Skip '^'
        std::string content;
        while (!isAtEnd() && peek() != '\n') {
            content += advance();
        }
        if (!isAtEnd()) {
            advance(); // Skip the newline
        }
        return Token(TokenType::SINGLE_COMMENT_START, content, currentLine, currentColumn);
    } else if (c == '/' && peek(1) == '@') {
        // Multi-line comment start
        advance(); // Skip '/'
        advance(); // Skip '@'
        return Token(TokenType::MULTI_COMMENT_START, "/@", currentLine, currentColumn);
    } else if (c == '@' && peek(1) == '/') {
        // Multi-line comment end
        advance(); // Skip '@'
        advance(); // Skip '/'
        return Token(TokenType::MULTI_COMMENT_END, "@/", currentLine, currentColumn);
    } else if (c == '#') {
        std::string include;
        advance(); // Skip '#'
        while (!isAtEnd() && !isspace(peek())) {
            include += advance();
        }
        if (include == "include") {
            skipWhitespace();
            std::string filename;
            while (!isAtEnd() && !isspace(peek())) {
                filename += advance();
            }
            handleInclude(filename);
            return getNextToken();
        }
        return errorToken("Invalid preprocessor directive");
    } else {
        // Handle operators and other single-character tokens
        token = scanOperator();
    }
    
    return token;
}

Token Scanner::scanIdentifier() {
    std::string value;
    int startColumn = currentColumn;
    
    while (!isAtEnd() && (isalnum(peek()) || peek() == '_')) {
        value += advance();
    }
    
    // Check if it's a type keyword
    if (value == "int" || value == "float" || value == "string" || value == "bool") {
        return Token(TokenType::TYPE, value, currentLine, startColumn);
    }
    
    // Check if it's a register (reg followed by number)
    if (value == "reg") {
        if (!isAtEnd() && isdigit(peek())) {
            value += advance(); // Add the number to the register name
            return Token(TokenType::REG, value, currentLine, startColumn);
        }
    }
    
    // Check if identifier starts with a number
    if (isdigit(value[0])) {
        return errorToken("Invalid identifier: " + value);
    }
    
    // Check if it's a boolean literal
    if (value == "true" || value == "false") {
        return Token(TokenType::BOOL_LITERAL, value, currentLine, startColumn);
    }
    
    auto it = keywords.find(value);
    if (it != keywords.end()) {
        return Token(it->second, value, currentLine, startColumn);
    }
    
    return Token(TokenType::IDENTIFIER, value, currentLine, startColumn);
}

Token Scanner::scanNumber() {
    std::string value;
    int startColumn = currentColumn;
    bool isFloat = false;
    
    while (!isAtEnd() && (isdigit(peek()) || peek() == '.')) {
        if (peek() == '.') {
            if (isFloat) {
                return errorToken("Invalid number format");
            }
            isFloat = true;
        }
        value += advance();
    }
    
    // If the number is followed by an identifier, it's an invalid identifier
    if (!isAtEnd() && (isalpha(peek()) || peek() == '_')) {
        std::string identifier = value;
        while (!isAtEnd() && (isalnum(peek()) || peek() == '_')) {
            identifier += advance();
        }
        return errorToken("Invalid identifier: " + identifier);
    }
    
    return Token(isFloat ? TokenType::FLOAT_LITERAL : TokenType::INTEGER_LITERAL,
                value, currentLine, startColumn);
}

Token Scanner::scanString() {
    std::string value;
    int startColumn = currentColumn;
    char quote = advance(); // Skip opening quote
    
    try {
        while (!isAtEnd()) {
            char c = peek();
            if (c == quote) {
                advance(); // Skip closing quote
                return Token(TokenType::STRING_LITERAL, value, currentLine, startColumn);
            } else if (c == '\\') {
                advance(); // Skip backslash
                if (isAtEnd()) {
                    return errorToken("Unterminated string after escape character");
                }
                switch (peek()) {
                    case 'n': value.push_back('\n'); break;
                    case 't': value.push_back('\t'); break;
                    case '\\': value.push_back('\\'); break;
                    case '"': value.push_back('"'); break;
                    case '\'': value.push_back('\''); break;
                    default: value.push_back('\\'); value.push_back(peek()); break;
                }
                advance();
            } else if (c == '\n') {
                return errorToken("Unterminated string - newline in string literal");
            } else {
                value.push_back(advance());
            }
        }
    } catch (const std::exception& e) {
        return errorToken("Error processing string literal");
    }
    
    return errorToken("Unterminated string");
}

Token Scanner::scanComment() {
    int startColumn = currentColumn;
    std::string content;
    
    while (!isAtEnd()) {
        if (peek() == '@' && peek(1) == '/') {
            return Token(TokenType::COMMENT_CONTENT, content, currentLine, startColumn);
        }
        content += advance();
    }
    
    return errorToken("Unterminated multi-line comment");
}

Token Scanner::scanOperator() {
    int startColumn = currentColumn;
    char c = advance();
    
    switch (c) {
        case '+': return Token(TokenType::PLUS, "+", currentLine, startColumn);
        case '-': return Token(TokenType::MINUS, "-", currentLine, startColumn);
        case '*': return Token(TokenType::MULTIPLY, "*", currentLine, startColumn);
        case '/': return Token(TokenType::DIVIDE, "/", currentLine, startColumn);
        case '=': 
            if (peek() == '=') {
                advance();
                return Token(TokenType::EQUAL, "==", currentLine, startColumn);
            }
            return Token(TokenType::ASSIGN, "=", currentLine, startColumn);
        case '!':
            if (peek() == '=') {
                advance();
                return Token(TokenType::NOT_EQUAL, "!=", currentLine, startColumn);
            }
            return Token(TokenType::NOT, "!", currentLine, startColumn);
        case '<':
            if (peek() == '=') {
                advance();
                return Token(TokenType::LESS_EQUAL, "<=", currentLine, startColumn);
            }
            return Token(TokenType::LESS, "<", currentLine, startColumn);
        case '>':
            if (peek() == '=') {
                advance();
                return Token(TokenType::GREATER_EQUAL, ">=", currentLine, startColumn);
            }
            return Token(TokenType::GREATER, ">", currentLine, startColumn);
        case '&':
            if (peek() == '&') {
                advance();
                return Token(TokenType::AND, "&&", currentLine, startColumn);
            }
            return errorToken("Invalid operator: " + std::string(1, c));
        case '|':
            if (peek() == '|') {
                advance();
                return Token(TokenType::OR, "||", currentLine, startColumn);
            }
            return errorToken("Invalid operator: " + std::string(1, c));
        case '(': return Token(TokenType::LEFT_PAREN, "(", currentLine, startColumn);
        case ')': return Token(TokenType::RIGHT_PAREN, ")", currentLine, startColumn);
        case '{': return Token(TokenType::LEFT_BRACE, "{", currentLine, startColumn);
        case '}': return Token(TokenType::RIGHT_BRACE, "}", currentLine, startColumn);
        case ';': return Token(TokenType::SEMICOLON, ";", currentLine, startColumn);
        case ',': return Token(TokenType::COMMA, ",", currentLine, startColumn);
        default:
            return errorToken("Unexpected character: " + std::string(1, c));
    }
}

void Scanner::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (isspace(c)) {
            if (advance() == '\n') {
                currentLine++;
                currentColumn = 0;
            }
        } else {
            break;
        }
    }
}

char Scanner::peek(int offset) {
    if (isAtEnd()) return '\0';
    
    char c = currentFile.peek();
    if (c == EOF) return '\0';
    return c;
}

char Scanner::advance() {
    if (isAtEnd()) return '\0';
    
    char c = currentFile.get();
    if (c == EOF) {
        currentFile.setstate(std::ios::eofbit);
        return '\0';
    }
    
    currentColumn++;
    return c;
}

bool Scanner::isAtEnd() {
    return currentFile.eof() || !currentFile.good();
}

void Scanner::handleInclude(const std::string& filename) {
    std::string includePath = filename;
    if (filename[0] != '/' && filename[0] != '\\') {
        // If not absolute path, make it relative to current file
        size_t lastSlash = currentFilename.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            includePath = currentFilename.substr(0, lastSlash + 1) + filename;
        }
    }
    
    std::ifstream includeFile(includePath);
    if (!includeFile.is_open()) {
        reportError("Could not open include file: " + filename);
        return;
    }
    
    // Save current file state
    std::ifstream oldFile = std::move(currentFile);
    std::string oldFilename = currentFilename;
    int oldLine = currentLine;
    int oldColumn = currentColumn;
    
    // Process include file
    currentFile = std::move(includeFile);
    currentFilename = includePath;
    currentLine = 1;
    currentColumn = 0;
    
    // Process all tokens from include file
    while (!isAtEnd()) {
        Token token = getNextToken();
        if (token.type == TokenType::END_OF_FILE) break;
    }
    
    // Restore previous file state
    currentFile = std::move(oldFile);
    currentFilename = oldFilename;
    currentLine = oldLine;
    currentColumn = oldColumn;
}

Token Scanner::errorToken(const std::string& message) {
    reportError(message);
    return Token(TokenType::ERROR, message, currentLine, currentColumn);
}

void Scanner::reportError(const std::string& message) {
    std::cerr << "Error at line " << currentLine << ", column " << currentColumn 
              << ": " << message << std::endl;
    errorCount++;
}

bool Scanner::hasError() const {
    return errorCount > 0;
}

int Scanner::getErrorCount() const {
    return errorCount;
} 