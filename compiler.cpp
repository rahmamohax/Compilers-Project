#include "compiler.h"
#include "scanner.h"
#include "symbol_table.h"
#include "token.h"
#include "parser.h"

#include <iostream>
#include <sstream>
#include <fstream>

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

std::string Compiler::readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool Compiler::compile(const std::string& sourceFile) {
    std::string source = readFile(sourceFile);
    if (source.empty()) {
        return false;
    }
    
    std::cout << "--- Compiling file: " << sourceFile << " ---\n";
    std::cout << "\n--- Source Code ---\n";
    std::cout << source << "\n";
    
    // Run Scanner
    Scanner scanner(source);
    auto tokens = scanner.scanTokens();

    std::cout << "\n--- Scanner Output ---\n";
    
    // Display scanner errors as part of the scanner output
    const auto& scannerErrors = scanner.getErrors();
    for (const auto& error : scannerErrors) {
        std::cout << "Scanner Error at line " << error.line << ": " << error.message << "\n";
    }
    
    // Display tokens
    for (const auto& token : tokens) {
        std::cout << "Line: " << token.line
                  << " Token Text: " << token.lexeme
                  << " Token Type: " << tokenTypeToString(token.type)
                  << "\n";
    }
    
    // Output scanner error count at the end of scanner output
    if (scanner.getErrorCount() > 0) {
        std::cout << "\nTotal scanner errors: " << scanner.getErrorCount() << "\n";
    }

    SymbolTable symtab;
    Parser parser(tokens, symtab);
    parser.parseProgram();
    
    std::cout << "\n--- Compilation Complete ---\n";
    return true;
}

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
        
        // Check if this is a file command
        if (line.substr(0, 5) == "file:") {
            std::string filename = line.substr(5);
            // Trim leading whitespace
            size_t start = filename.find_first_not_of(" \t");
            if (start != std::string::npos) {
                filename = filename.substr(start);
                compile(filename);
            }
            continue;
        }
        
        buffer << line << '\n';
    }

    // If there's any input in the buffer, process it
    std::string source = buffer.str();
    if (!source.empty()) {
        // Run Scanner
        Scanner scanner(source);
        auto tokens = scanner.scanTokens();

        std::cout << "\n--- Scanner Output ---\n";
        
        // Display scanner errors as part of the scanner output
        const auto& scannerErrors = scanner.getErrors();
        for (const auto& error : scannerErrors) {
            std::cout << "Scanner Error at line " << error.line << ": " << error.message << "\n";
        }
        
        // Display tokens
        for (const auto& token : tokens) {
            std::cout << "Line: " << token.line
                    << " Token Text: " << token.lexeme
                    << " Token Type: " << tokenTypeToString(token.type)
                    << "\n";
        }
        
        // Output scanner error count at the end of scanner output
        if (scanner.getErrorCount() > 0) {
            std::cout << "\nTotal scanner errors: " << scanner.getErrorCount() << "\n";
        }

        SymbolTable symtab;
        Parser parser(tokens, symtab);
        parser.parseProgram();
    }
}
