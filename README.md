# Compiler Project

## Overview
This project implements a compiler for a custom programming language, featuring lexical analysis, parsing, and symbol table management. The compiler processes source code written in the custom language, performs lexical and syntactic analysis, and manages variable and function declarations using a symbol table.

## Project Structure
The project consists of the following source files:
- **main.cpp**: Entry point of the program, initializes the compiler and handles command-line or interactive input.
- **compiler.cpp**: Core compiler logic, orchestrates file reading, scanning, and parsing.
- **scanner.cpp**: Lexical analyzer, converts source code into tokens.
- **parser.cpp**: Syntax analyzer, processes tokens to ensure syntactic correctness and manages declarations.
- **symbol_table.cpp**: Manages variable and function declarations with scoping.
- **token.cpp**: Defines token types and provides utility functions for token handling.
- **Header files** (`*.h`): Define classes, enums, and function prototypes for the above components.

## Features
- **Lexical Analysis**: Identifies tokens such as keywords, identifiers, constants, and operators.
- **Syntax Analysis**: Parses tokens to ensure valid syntax, including variable declarations, function definitions, and statements.
- **Symbol Table**: Tracks variable and function declarations with support for scoping.
- **Error Handling**: Reports lexical and syntactic errors with line numbers.
- **Interactive Mode**: Allows users to input code directly or compile from a file.

## Prerequisites
- C++ compiler (e.g., g++, MSVC)
- CMake (optional, for building)
- IDE: Visual Studio, Code::Blocks, or Visual Studio Code
- Standard C++ libraries

## Building the Project
### Using Visual Studio
1. Open Visual Studio.
2. Create a new project or open an existing one.
3. Add all `.cpp` and `.h` files to the project.
4. Configure the project to use C++17 or later.
5. Build the solution (Debug or Release mode).
6. The executable will be generated in the `Debug` or `Release` folder (e.g., `compiler.exe`).

### Using Code::Blocks
1. Open Code::Blocks.
2. Create a new project (Console Application).
3. Add all `.cpp` and `.h` files to the project.
4. Ensure the compiler is set to a C++ compiler (e.g., GCC).
5. Build the project (Debug or Release target).
6. The executable will be generated in the `bin/Debug` or `bin/Release` folder (e.g., `compiler.exe`).
7. Alternatively, open the provided `compiler.cbp` file and build it directly.

### Using Visual Studio Code
1. Install the C/C++ extension and CMake Tools (if using CMake).
2. Open the project folder in VS Code.
3. If using CMake:
   - Create a `CMakeLists.txt` file (see below for an example).
   - Run `CMake: Configure` and `CMake: Build` from the Command Palette.
4. If building manually:
   - Open a terminal in VS Code.
   - Navigate to the project directory.
   - Compile using g++:
     ```bash
     g++ -std=c++17 *.cpp -o compiler
     ```
5. The executable (`compiler` or `compiler.exe`) will be generated in the project directory.

**Example CMakeLists.txt**:
```cmake
cmake_minimum_required(VERSION 3.10)
project(Compiler)
set(CMAKE_CXX_STANDARD 17)
add_executable(compiler main.cpp compiler.cpp scanner.cpp parser.cpp symbol_table.cpp token.cpp)
```

## Running the Compiler
### In Visual Studio
1. Build the project (see above).
2. Run the program in Debug mode (F5) or Release mode.
3. To compile a file, pass the filename as a command-line argument in the project settings or run the executable manually:
   ```bash
   .\Debug\compiler.exe input.txt
   ```
4. For interactive mode, run without arguments:
   ```bash
   .\Debug\compiler.exe
   ```

### In Code::Blocks
1. Build the project using the `compiler.cbp` file or your project.
2. Run the program (F9) for interactive mode.
3. To compile a file, pass the filename as a command-line argument in the project settings or run manually:
   ```bash
   .\bin\Debug\compiler.exe input.txt
   ```
4. For interactive mode, run without arguments:
   ```bash
   .\bin\Debug\compiler.exe
   ```

### In Visual Studio Code
1. Build the project (see above).
2. Open a terminal in VS Code.
3. Navigate to the build directory (if using CMake) or project directory.
4. Run the compiler:
   ```bash
   ./compiler input.txt
   ```
   or, for Windows:
   ```bash
   .\compiler.exe input.txt
   ```
5. For interactive mode, run without arguments:
   ```bash
   ./compiler
   ```
6. Alternatively, configure a VS Code task to run the compiler:
   - Create a `.vscode/tasks.json` file:
     ```json
     {
       "version": "2.0.0",
       "tasks": [
         {
           "label": "Run Compiler",
           "type": "shell",
           "command": "./compiler",
           "group": {
             "kind": "build",
             "isDefault": true
           }
         }
       ]
     }
     ```
   - Run the task using `Ctrl+Shift+B`.

## Usage
- **Interactive Mode**:
  - Run the compiler without arguments.
  - Enter code line by line.
  - Type `end` to finish input and process the code.
  - Use `file:filename` to compile a file (e.g., `file:input.txt`).
- **File Mode**:
  - Pass a filename as a command-line argument.
  - The compiler will read and process the file.
- **Output**:
  - Displays source code, scanner output (tokens), parser output (matched rules), and any errors.

## Parser and Token Rules
### Token Rules
The scanner (`scanner.cpp`) recognizes the following lexemes:
- **Keywords**:
  - `IfTrue`, `Otherwise` (conditionals)
  - `Imw` (Integer), `SIMw` (SInteger), `IMwf` (Float), `SIMwf` (SFloat)
  - `Chj` (Character), `Series` (String), `NOReturn` (Void)
  - `RepeatWhen`, `Reiterate` (loops)
  - `Turnback` (return), `OutLoop` (break)
  - `Loli` (struct), `Include` (include)
- **Identifiers**: Alphanumeric sequences starting with a letter or underscore.
- **Constants**:
  - Integer: `123`, `+123`, `-123`
  - Float: `123.456`, `+123.456`, `-123.456`
  - Character: Not explicitly shown in scanner (assumed to be handled similarly).
  - String: Not explicitly shown in scanner (assumed to be handled similarly).
- **Operators**:
  - Arithmetic: `+`, `-`, `*`, `/`
  - Comparison: `<`, `>`, `<=`, `>=`, `==`, `!=`
  - Logical: `&&` (`And`), `||` (`Or`), `~` (`Not`)
  - Assignment: `=`
- **Punctuation**: `{`, `}`, `[`, `]`, `(`, `)`, `;`, `,`
- **Comments**:
  - Single-line: `/^` followed by content until newline.
  - Multi-line: `/@` to `@/`.
- **Invalid**: Identifiers starting with digits or unrecognized characters.

### Parser Rules
The parser (`parser.cpp`) enforces the following syntactic rules:
- **Program**: Sequence of declarations, function definitions, or statements.
- **Declarations**:
  - Format: `type identifier [ = expression ] [, identifier [ = expression ]]* ;`
  - Types: `Imw`, `SIMw`, `Chj`, `Series`, `IMwf`, `SIMwf`.
  - Checks for redeclaration and type compatibility.
- **Function Definitions**:
  - Format: `type identifier ( [type identifier [, type identifier]*] ) { statements }`
  - Supports parameters and scoping.
  - Checks for redeclaration and valid return types.
- **Statements**:
  - Assignment: `identifier = expression ;`
  - Selection: `IfTrue ( expression ) statement [ Otherwise statement ]`
  - Iteration: `RepeatWhen ( expression ) statement` or `Reiterate ( expression ) statement`
  - Jump: `Turnback [ expression ] ;` or `OutLoop ;`
  - Block: `{ statements }`
  - Expression statement: `expression ;`
- **Expressions**:
  - Logical: `||`, `&&`
  - Comparison: `<`, `>`, `<=`, `>=`, `==`, `!=`
  - Arithmetic: `+`, `-`, `*`, `/`
  - Factors: Identifiers, constants, or parenthesized expressions.
- **Comments**: Skipped during parsing but logged.
- **Error Handling**:
  - Reports syntax errors with line numbers.
  - Synchronizes after errors to continue parsing.
  - Checks for undeclared variables and type mismatches.

## Example Input
```plaintext
Imw x = 42;
SIMwf y = -3.14;
IfTrue (x > 0) {
    Series msg = "Positive";
}
NOReturn main() {
    Turnback;
}
```

## Limitations
- Limited type checking for complex expressions.
- No code generation or semantic analysis.
- Structs (`Loli`) and includes (`Include`) are recognized but not fully implemented.
