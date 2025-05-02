#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <stack>
#include <cctype>
#include <iomanip>

using namespace std;

// Token types for the scanner
enum class TokenType {
    COMMENT_START,    // /@
    COMMENT_END,      // @/
    COMMENT_CONTENT,  // Comment content
    VOID,            // NOReturn
    IDENTIFIER,      // Variable/function names
    TYPE,            // int, IMw, etc.
    BRACES,          // (, ), {, }
    OPERATOR,        // =, <, >, etc.
    CONSTANT,        // Numeric constants
    ASSIGNMENT,      // =
    SEMICOLON,       // ;
    INVALID,         // Invalid tokens
    END_OF_FILE
};

struct Token {
    TokenType type;
    string lexeme;
    int line;

    Token() : type(TokenType::END_OF_FILE), lexeme(""), line(0) {}
    Token(TokenType t, const string& l, int ln)
        : type(t), lexeme(l), line(ln) {}
};

class Scanner {
private:
    string input;
    size_t position;
    int line;
    int errorCount;
    vector<Token> tokens;

    void addToken(TokenType type, const string& lexeme) {
        tokens.push_back(Token(type, lexeme, line));
    }

public:
    Scanner() : position(0), line(1), errorCount(0) {
        cout << "Enter your code (Enter 'END' on a new line to finish):\n";
        string line;
        while (getline(cin, line)) {
            if (line == "END") break;
            input += line + "\n";
        }
        scanTokens();
    }

    void scanTokens() {
        while (position < input.length()) {
            // Skip whitespace
            while (position < input.length() && isspace(input[position])) {
                if (input[position] == '\n') line++;
                position++;
            }

            if (position >= input.length()) break;

            // Check for comments
            if (position + 1 < input.length() && input.substr(position, 2) == "/@") {
                position += 2;
                addToken(TokenType::COMMENT_START, "/@");

                size_t end = input.find("@/", position);
                if (end != string::npos) {
                    string content = input.substr(position, end - position);
                    addToken(TokenType::COMMENT_CONTENT, content);
                    position = end;
                    addToken(TokenType::COMMENT_END, "@/");
                    position += 2;
                }
                continue;
            }

            // Check for NOReturn
            if (position + 8 <= input.length() && input.substr(position, 8) == "NOReturn") {
                addToken(TokenType::VOID, "NOReturn");
                position += 8;
                continue;
            }

            // Check for types
            if (position + 3 <= input.length() && input.substr(position, 3) == "int") {
                addToken(TokenType::TYPE, "int");
                position += 3;
                continue;
            }

            // Check for numbers or invalid identifiers starting with numbers
            if (isdigit(input[position])) {
                string token;
                size_t start_pos = position;

                // Collect the full token
                while (position < input.length() &&
                       (isalnum(input[position]) || input[position] == '_')) {
                    token += input[position++];
                }

                // If it contains only digits, it's a valid constant
                bool isOnlyDigits = true;
                for (char c : token) {
                    if (!isdigit(c)) {
                        isOnlyDigits = false;
                        break;
                    }
                }

                if (isOnlyDigits) {
                    addToken(TokenType::CONSTANT, token);
                } else {
                    errorCount++;
                    addToken(TokenType::INVALID, token);
                }
                continue;
            }

            // Check for identifiers (must start with letter or underscore)
            if (isalpha(input[position]) || input[position] == '_') {
                string identifier;
                while (position < input.length() &&
                       (isalnum(input[position]) || input[position] == '_')) {
                    identifier += input[position++];
                }
                addToken(TokenType::IDENTIFIER, identifier);
                continue;
            }

            // Check for assignment operator
            if (input[position] == '=') {
                addToken(TokenType::ASSIGNMENT, "=");
                position++;
                continue;
            }

            // Check for braces and other symbols
            switch (input[position]) {
                case '(':
                case ')':
                case '{':
                case '}':
                    addToken(TokenType::BRACES, string(1, input[position]));
                    position++;
                    continue;
                case ';':
                    addToken(TokenType::SEMICOLON, ";");
                    position++;
                    continue;
            }

            position++;
        }
    }

    void printScannerOutput() {
        cout << "\nScanner Output:" << endl;
        for (const Token& token : tokens) {
            cout << "Line : " << token.line << " Token Text: " << left << setw(15)
                     << token.lexeme;

            switch (token.type) {
                case TokenType::COMMENT_START:
                    cout << "Token Type: Comment Start";
                    break;
                case TokenType::COMMENT_CONTENT:
                    cout << "Token Type: Comment Content";
                    break;
                case TokenType::COMMENT_END:
                    cout << "Token Type: Comment End";
                    break;
                case TokenType::VOID:
                    cout << "Token Type: Void";
                    break;
                case TokenType::IDENTIFIER:
                    cout << "Token Type: Identifier";
                    break;
                case TokenType::TYPE:
                    cout << "Token Type: Type";
                    break;
                case TokenType::BRACES:
                    cout << "Token Type: Braces";
                    break;
                case TokenType::ASSIGNMENT:
                    cout << "Token Type: Assignment operator";
                    break;
                case TokenType::CONSTANT:
                    cout << "Token Type: Constant";
                    break;
                case TokenType::INVALID:
                    cout << "Token Type: Invalid Identifier";
                    break;
                default:
                    break;
            }
            cout << endl;
        }

        if (errorCount > 0) {
            cout << "\nTotal NO of errors: " << errorCount << endl;
        }
    }

    const vector<Token>& getTokens() const { return tokens; }
    int getErrorCount() const { return errorCount; }
};

class Parser {
private:
    const vector<Token>& tokens;
    size_t currentToken;
    int errorCount;

    void reportError(const string& message, int line) {
        cout << "Line : " << line << " Not Matched\t\tError: " << message << endl;
        errorCount++;
    }

    void reportMatch(const string& rule, int line) {
        cout << "Line : " << line << " Matched\t\tRule used: " << rule << endl;
    }

public:
    Parser(const vector<Token>& t) : tokens(t), currentToken(0), errorCount(0) {}

    void parse() {
        cout << "\nParser Phase Output:" << endl;

        for (size_t i = 0; i < tokens.size(); i++) {
            const Token& token = tokens[i];

            switch (token.type) {
                case TokenType::COMMENT_START:
                    reportMatch("Comment", token.line);
                    break;
                case TokenType::VOID:
                    reportMatch("fun-declaration", token.line);
                    break;
                case TokenType::INVALID:
                    reportError("Invalid identifier \"" + token.lexeme + "\"", token.line);
                    break;
                default:
                    break;
            }
        }

        cout << "\nTotal NO of errors: " << errorCount << endl;
    }
};

int main() {
    Scanner scanner;
    scanner.printScannerOutput();

    Parser parser(scanner.getTokens());
    parser.parse();

    return 0;
}
