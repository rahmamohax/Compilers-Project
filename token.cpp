#include "token.h"
using namespace std;
string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::Condition: return "Condition";
        case TokenType::Integer: return "Integer";
        case TokenType::SInteger: return "SInteger";
        case TokenType::SignedIntegerConstant: return"INTgerSIgned";
        case TokenType::SignedFloatConstant: return"FloatSIgned";
        case TokenType::CharConstant:return "CharConstant";
        case TokenType::StringConstant:return"StringConstant";

        case TokenType::Character: return "Character";
        case TokenType::String: return "String";
        case TokenType::Float: return "Float";
        case TokenType::SFloat: return "SFloat";
        case TokenType::Void: return "Void";
        case TokenType::Loop: return "Loop";
        case TokenType::Return: return "Return";
        case TokenType::Break: return "Break";
        case TokenType::Struct: return "Struct";
        case TokenType::Include: return "Include";

        case TokenType::Plus: return "Plus";
        case TokenType::Minus: return "Minus";
        case TokenType::Multiply: return "Multiply";
        case TokenType::Divide: return "Divide";

        case TokenType::And: return "And";
        case TokenType::Or: return "Or";
        case TokenType::Not: return "Not";

        case TokenType::Equal: return "Equal";
        case TokenType::Less: return "Less";
        case TokenType::Greater: return "Greater";
        case TokenType::NotEqual: return "NotEqual";
        case TokenType::LessEqual: return "LessEqual";
        case TokenType::GreaterEqual: return "GreaterEqual";

        case TokenType::Assignment: return "Assignment";
        case TokenType::Access: return "Access";

        case TokenType::LeftBrace: return "LeftBrace";
        case TokenType::RightBrace: return "RightBrace";
        case TokenType::LeftBracket: return "LeftBracket";
        case TokenType::RightBracket: return "RightBracket";
        case TokenType::LeftParen: return "LeftParen";
        case TokenType::RightParen: return "RightParen";
        case TokenType::Semicolon: return "Semicolon";
        case TokenType::Comma: return "Comma";

        case TokenType::Identifier: return "Identifier";
        case TokenType::IntgerConstant: return "IntConstant";
        case TokenType::FloatConstant: return "FloatConstant";

        case TokenType::SMultiComment: return "SMulticomment";
        case TokenType::EMultiComment: return "EMulticomment";
        case TokenType::SingleComment: return "Singlecomment";
        case TokenType::CommentContent: return "CommentContent";

        case TokenType::EndOfFile: return "EndOfFile";
        case TokenType::Invalid: return "Invalid";
        default: return "Unknown";
    }
}
