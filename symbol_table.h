#ifndef SYMBOL_TABLE_H_INCLUDED
#define SYMBOL_TABLE_H_INCLUDED

#include <string>
#include <vector>
#include <map>

using std::string;
using std::vector;
using std::map;

enum class SymbolType {
    Integer,
    SInteger,
    Float,
    SFloat,
    Character,
    String,
    Void,
    Unknown,
};

class SymbolTable {
public:
    SymbolTable() = default;

    // Structure to store function signature
    struct FunctionSignature {
        SymbolType returnType;
        vector<SymbolType> paramTypes;
        FunctionSignature(SymbolType ret, const vector<SymbolType>& params)
            : returnType(ret), paramTypes(params) {}
    };

    // Enter a new scope (e.g., for function body)
    void enterScope();

    // Exit the current scope
    void exitScope();

    // Declare a variable with a given name and type
    bool declareVariable(const string& name, SymbolType type);

    // Declare a function with name, return type, and parameter types
    bool declareFunction(const string& name, SymbolType returnType, const vector<SymbolType>& paramTypes);

    // Check if a variable exists in the current or outer scopes
    bool exists(const string& name) const;

    // Check if a function exists
    bool functionExists(const string& name) const;

    // Get the type of a variable
    SymbolType getVariableType(const string& name) const;

    // Get the function signature
    FunctionSignature getFunctionSignature(const string& name) const;

    // Convert SymbolType to string for error messages
    string typeToString(SymbolType type) const;

private:
    // Vector of maps for scoped variable storage
    vector<map<string, SymbolType>> variableScopes;

    // Map to store function names and their signatures
    map<string, FunctionSignature> functions;
};

#endif // SYMBOL_TABLE_H_INCLUDED
