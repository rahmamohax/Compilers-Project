#ifndef COMPILER_H_INCLUDED
#define COMPILER_H_INCLUDED
#include <string>

class Compiler {
public:
    bool compile(const std::string& sourceFile);
    void run();
private:
    std::string readFile(const std::string& filename);
};


#endif // COMPILER_H_INCLUDED
