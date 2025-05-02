#ifndef Token_include
#define Token_include

#include <string>
using namespace std;

enum class TokenType
{
    // Keywords
    Condition,
    Integer,
    SInteger,
    Character,
    String,
    Float,
    SFloat,
    Void,
    Loop,
    Return,
    Break,
    Struct,
    Include,

    // Operators
    Plus,
    Minus,
    Multiply,
    Divide,

    Equal,
    Less,
    Greater,
    NotEqual,
    LessEqual,
    GreaterEqual,
    Assignment,
    Access,               // = and ->
    Arithmetic_Operation, // Grouped for all Airthmetic_Operation

    // logical operation
    And,               // &&
    Or,                // ||
    Not,               // ~
    Logical_Operation, // LogicalOperation

    // Braces & Delimiters
    LeftBrace,
    RightBrace,
    LeftBracket,
    RightBracket,
    LeftParen,
    RightParen,
    Semicolon,
    Comma,

    // Literals & Identifiers
    Identifier,
    IntgerConstant, // 0...9
    FloatConstant, // 0.0 ... 9.9

    // Comments
    SMultiComment, // /@
    EMultiComment, // @/
    SingleComment, // /^
    CommentContent,

    // Special
    EndOfFile,
    Invalid
};

struct Token
{
    TokenType type;
    string lexeme;
    int line;

    Token(TokenType type, const string &lexeme, int line)
        : type(type), lexeme(lexeme), line(line) {}
};

string tokenTypeToString(TokenType type);

#endif
