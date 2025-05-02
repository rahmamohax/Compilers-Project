#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <vector>

enum class SymbolKind {
    Variable,
    Function,
    Struct
};

enum class SymbolType {
    Integer,
    SInteger,
    Character,
    String,
    Float,
    SFloat,
    Void,
    Unknown
};

struct Symbol {
    std::string name;
    SymbolKind kind;
    SymbolType type;
    std::vector<SymbolType> paramTypes;  // For functions

    Symbol(const std::string& name, SymbolKind kind, SymbolType type)
        : name(name), kind(kind), type(type) {}
};

class SymbolTable {
public:
    bool declareVariable(const std::string& name, SymbolType type);
    bool declareFunction(const std::string& name, SymbolType returnType, const std::vector<SymbolType>& params);
    bool declareStruct(const std::string& name);

    bool exists(const std::string& name);
    Symbol* lookup(const std::string& name);

    void printTable();

private:
    std::unordered_map<std::string, Symbol> table;
};

#endif
