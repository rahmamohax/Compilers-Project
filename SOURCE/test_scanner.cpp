#include <iostream>
#include "../HEADERS/scanner.h"
using namespace std;
int main() {
    Scanner scanner;
    if (!scanner.openFile("test_input.txt")) {
     cerr << "Failed to open test file\n";
        return 1;
    }

 cout << "Testing scanner...\n";
 cout << "Token\t\tType\t\tLine\tColumn\n";
 cout << "----------------------------------------\n";

    Token token;
    do {
        token = scanner.getNextToken();
     cout << token.value << "\t\t";
        
        switch (token.type) {
            case TokenType::NORETURN: std::cout << "NORETURN"; break;
            case TokenType::FLOAT: std::cout << "FLOAT"; break;
            case TokenType::STRING: std::cout << "STRING"; break;
            case TokenType::BOOL: std::cout << "BOOL"; break;
            case TokenType::IF_TRUE: std::cout << "IF_TRUE"; break;
            case TokenType::OTHERWISE: std::cout << "OTHERWISE"; break;
            case TokenType::REPEATWHEN: std::cout << "REPEATWHEN"; break;
            case TokenType::RETURN: std::cout << "RETURN"; break;
            case TokenType::IDENTIFIER: std::cout << "IDENTIFIER"; break;
            case TokenType::INTEGER_LITERAL: std::cout << "INTEGER_LITERAL"; break;
            case TokenType::FLOAT_LITERAL: std::cout << "FLOAT_LITERAL"; break;
            case TokenType::STRING_LITERAL: std::cout << "STRING_LITERAL"; break;
            case TokenType::BOOL_LITERAL: std::cout << "BOOL_LITERAL"; break;
            case TokenType::PLUS: std::cout << "PLUS"; break;
            case TokenType::MINUS: std::cout << "MINUS"; break;
            case TokenType::MULTIPLY: std::cout << "MULTIPLY"; break;
            case TokenType::DIVIDE: std::cout << "DIVIDE"; break;
            case TokenType::EQUAL: std::cout << "EQUAL"; break;
            case TokenType::NOT_EQUAL: std::cout << "NOT_EQUAL"; break;
            case TokenType::LESS: std::cout << "LESS"; break;
            case TokenType::GREATER: std::cout << "GREATER"; break;
            case TokenType::LESS_EQUAL: std::cout << "LESS_EQUAL"; break;
            case TokenType::GREATER_EQUAL: std::cout << "GREATER_EQUAL"; break;
            case TokenType::ASSIGN: std::cout << "ASSIGN"; break;
            case TokenType::LEFT_PAREN: std::cout << "LEFT_PAREN"; break;
            case TokenType::RIGHT_PAREN: std::cout << "RIGHT_PAREN"; break;
            case TokenType::LEFT_BRACE: std::cout << "LEFT_BRACE"; break;
            case TokenType::RIGHT_BRACE: std::cout << "RIGHT_BRACE"; break;
            case TokenType::SEMICOLON: std::cout << "SEMICOLON"; break;
            case TokenType::COMMA: std::cout << "COMMA"; break;
            case TokenType::ERROR: std::cout << "ERROR"; break;
            case TokenType::END_OF_FILE: std::cout << "END_OF_FILE"; break;
            default: std::cout << "UNKNOWN"; break;
        }
        
     cout << "\t\t" << token.line << "\t" << token.column << "\n";
    } while (token.type != TokenType::END_OF_FILE);

    if (scanner.hasError()) {
     cout << "\nScanner found " << scanner.getErrorCount() << " errors\n";
        return 1;
    }

 cout << "\nScanner test completed successfully\n";
    return 0;
} 