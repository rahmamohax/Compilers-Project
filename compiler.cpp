#include "compiler.h"
#include "scanner.h"
#include "symbol_table.h"
#include "token.h"
#include "parser.h"

#include <iostream>
#include <sstream>

SymbolType mapTokenTypeToSymbolType(TokenType tokenType) {
    switch (tokenType) {
        case TokenType::Integer: return SymbolType::Integer;
        case TokenType::SInteger: return SymbolType::SInteger;
        case TokenType::Character: return SymbolType::Character;
        case TokenType::String: return SymbolType::String;
        case TokenType::Float: return SymbolType::Float;
        case TokenType::SFloat: return SymbolType::SFloat;
        case TokenType::Void: return SymbolType::Void;
        default: return SymbolType::Unknown;
    }
}

void handleDeclarations(const vector<Token>& tokens, SymbolTable& symtab) {
    for (size_t i = 0; i < tokens.size(); ++i) {
        const Token& tok = tokens[i];

        // If we see type-specifier
        if (tok.type == TokenType::Integer || tok.type == TokenType::SInteger ||
            tok.type == TokenType::Character || tok.type == TokenType::String ||
            tok.type == TokenType::Float || tok.type == TokenType::SFloat ||
            tok.type == TokenType::Void) {

            SymbolType varType = mapTokenTypeToSymbolType(tok.type);
            i++; // move to next token

            // Collect identifiers
            while (i < tokens.size() && tokens[i].type == TokenType::Identifier) {
                string varName = tokens[i].lexeme;

                // Check redeclaration
                if (!symtab.declareVariable(varName, varType)) {
                    cerr << "❌ Error: Variable '" << varName
                              << "' already declared (line " << tokens[i].line << ")\n";
                }

                i++;
                if (i < tokens.size() && tokens[i].type == TokenType::Comma) {
                    i++; // skip comma and continue
                } else {
                    break; // no more IDs
                }
            }
        }
    }
}

// <-- Don't forget to include parser.h at top!

void Compiler::run() {
    std::cout << "Enter your Project#3 code (type 'end' alone to finish input):\n";

    std::ostringstream buffer;
    std::string line;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        if (line == "end") {
            break;
        }
        buffer << line << '\n';
    }

    std::string source = buffer.str();

    // 1️⃣ Run Scanner
    Scanner scanner(source);
    auto tokens = scanner.scanTokens();

    std::cout << "\n--- Scanner Output ---\n";
    for (const auto& token : tokens) {
        std::cout << "Line: " << token.line
                  << " Token Text: " << token.lexeme
                  << " Token Type: " << tokenTypeToString(token.type)
                  << "\n";
    }

  SymbolTable symtab;
    Parser parser(tokens, symtab);
    parser.parseProgram();
    // 4️⃣ (Optional) Print final symbol table

}
