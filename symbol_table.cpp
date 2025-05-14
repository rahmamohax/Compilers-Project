#include "symbol_table.h"
#include <stdexcept>
#include <vector>
#include <map>

using std::vector;

void SymbolTable::enterScope() {
    variableScopes.emplace_back();
}

void SymbolTable::exitScope() {
    if (!variableScopes.empty()) {
        variableScopes.pop_back();
    }
}

bool SymbolTable::declareVariable(const string& name, SymbolType type) {
    if (variableScopes.empty()) {
        variableScopes.emplace_back();
    }
    auto& currentScope = variableScopes.back();
    if (currentScope.find(name) != currentScope.end() || functions.find(name) != functions.end()) {
        return false; // Variable or function already declared
    }
    currentScope[name] = type;
    return true;
}

bool SymbolTable::declareFunction(const string& name, SymbolType returnType, const vector<SymbolType>& paramTypes) {
    if (functions.find(name) != functions.end() ||
        (!variableScopes.empty() && variableScopes.back().find(name) != variableScopes.back().end())) {
        return false; // Function or variable already declared
    }
    functions.emplace(name, FunctionSignature(returnType, paramTypes));
    return true;
}

bool SymbolTable::exists(const string& name) const {
    for (auto it = variableScopes.rbegin(); it != variableScopes.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return true;
        }
    }
    return false;
}

bool SymbolTable::functionExists(const string& name) const {
    return functions.find(name) != functions.end();
}

SymbolType SymbolTable::getVariableType(const string& name) const {
    for (auto it = variableScopes.rbegin(); it != variableScopes.rend(); ++it) {
        auto varIt = it->find(name);
        if (varIt != it->end()) {
            return varIt->second;
        }
    }
    throw std::runtime_error("Variable '" + name + "' not found");
}

SymbolTable::FunctionSignature SymbolTable::getFunctionSignature(const string& name) const {
    auto it = functions.find(name);
    if (it == functions.end()) {
        throw std::runtime_error("Function '" + name + "' not found");
    }
    return it->second;
}

string SymbolTable::typeToString(SymbolType type) const {
    switch (type) {
        case SymbolType::Integer: return "Integer";
        case SymbolType::SInteger: return "SInteger";
        case SymbolType::Float: return "Float";
        case SymbolType::SFloat: return "SFloat";
        case SymbolType::Character: return "Character";
        case SymbolType::String: return "String";
        case SymbolType::Void: return "Void";
        default: return "Unknown";
    }
}
