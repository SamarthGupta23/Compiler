#include "common.hpp"
#include "chunk.hpp"
#include "debug.hpp"
#include "value.hpp"
#include "vm.hpp"
#include "result.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

VM vm;

static void repl() {
    std::string line;

    for (;;) {
        std::cout << "> ";

        if (!std::getline(std::cin, line)) {
            std::cout << "\n";
            break;
        }

        vm.interpret(line);
    }

}

static void writeChunkToFile(Chunk& chunk, std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Could not open " << filename << " for writing!\n";
        return;
    }
    out << "Opcode Array:\n";
    const auto& code = chunk.getCode();
    for (size_t i = 0; i < code.size(); ++i) {
        out << i << ": " << static_cast<int>(code[i]);
        // Print constant index if this is a two-byte opcode
        if (code[i] == Opcode::OP_CONSTANT || code[i] == Opcode::OP_DEFINE_GLOBAL ||
            code[i] == Opcode::OP_GET_GLOBAL || code[i] == Opcode::OP_SET_GLOBAL) {
            out << " [" << static_cast<int>(code[i+1]) << "]";
            ++i;
            }
        out << "\n";
    }
    out << "Constants:\n";
    const auto& constants = chunk.getValueArray().ValueVector;
    for (size_t i = 0; i < constants.size(); ++i) {
        out << i << ": ";
        chunk.constants.getIndex(i).printValue();
        out << "\n";
    }
    out.close();
}

static std::string readFile(const char* path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Could not open file \"" << path << "\".\n";
        std::exit(74);
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static void runFile(const char* path) {
    std::string source = readFile(path);
    std::cout << "Source code read successfully, length: " << source.length() << "\n";

    InterpretResult result = vm.interpret(source);
    std::cout << "Interpretation result: " << static_cast<int>(result) << "\n";

    if (result == InterpretResult::INTERPRET_COMPILE_ERROR) {
        std::cout << "Compile error occurred\n";
        std::exit(65);
    }
    if (result == InterpretResult::INTERPRET_RUNTIME_ERROR) {
        std::cout << "Runtime error occurred\n";
        std::exit(70);
    }

    std::cout << "Execution completed successfully\n";
}

int main(int argc, const char* argv[]) {
    vm.initVM();

    // Always try to run code.lol first
    const char* defaultFile = "C:\\Users\\samar\\CLionProjects\\cppcompiler\\code.lol" ;

    // Check if code.lol exists at absolute path
    std::ifstream testFile(defaultFile);
    if (testFile.good()) {
        testFile.close();
        std::cout << "Running " << defaultFile << "...\n";
        runFile(defaultFile);
    } else {
        std::cout << "code.lol not found at specified path, checking command line arguments...\n";

        if (argc == 1) {
            std::cout << "No arguments provided, starting REPL...\n";
            repl();
        } else if (argc == 2) {
            std::cout << "Running file: " << argv[1] << "\n";
            runFile(argv[1]);
        } else {
            std::cerr << "Usage: clox [path]\n";
            std::exit(64);
        }
    }

    return 0;
}