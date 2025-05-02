#include "symbol_table.h"
#include <iostream>

bool SymbolTable::declareVariable(const std::string& name, SymbolType type) {
    if (exists(name)) return false;
    table.emplace(name, Symbol(name, SymbolKind::Variable, type));
    return true;
}

bool SymbolTable::declareFunction(const std::string& name, SymbolType returnType, const std::vector<SymbolType>& params) {
    if (exists(name)) return false;
    Symbol sym(name, SymbolKind::Function, returnType);
    sym.paramTypes = params;
    table.emplace(name, sym);
    return true;
}

bool SymbolTable::declareStruct(const std::string& name) {
    if (exists(name)) return false;
    table.emplace(name, Symbol(name, SymbolKind::Struct, SymbolType::Unknown));
    return true;
}

bool SymbolTable::exists(const std::string& name) {
    return table.find(name) != table.end();
}

Symbol* SymbolTable::lookup(const std::string& name) {
    auto it = table.find(name);
    if (it != table.end()) {
        return &(it->second);
    }
    return nullptr;
}

void SymbolTable::printTable() {
    std::cout << "\n----- Symbol Table -----\n";
    for (const auto& pair : table) {
        const std::string& name = pair.first;
        const Symbol& symbol = pair.second;

        std::cout << "Name: " << name
                  << ", Kind: " << (symbol.kind == SymbolKind::Variable ? "Var" :
                                   symbol.kind == SymbolKind::Function ? "Func" : "Struct")
                  << ", Type: ";

        switch (symbol.type) {
            case SymbolType::Integer: std::cout << "Integer"; break;
            case SymbolType::SInteger: std::cout << "SInteger"; break;
            case SymbolType::Character: std::cout << "Character"; break;
            case SymbolType::String: std::cout << "String"; break;
            case SymbolType::Float: std::cout << "Float"; break;
            case SymbolType::SFloat: std::cout << "SFloat"; break;
            case SymbolType::Void: std::cout << "Void"; break;
            default: std::cout << "Unknown"; break;
        }

        std::cout << "\n";
    }
    std::cout << "------------------------\n";
}

