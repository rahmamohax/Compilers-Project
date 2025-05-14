#include "compiler.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Compiler compiler;
    
    if (argc > 1) {
        // If a file is specified on the command line, compile it directly
        compiler.compile(argv[1]);
    } else {
        // Otherwise, run the interactive mode
        compiler.run();
    }
    
    return 0;
}
