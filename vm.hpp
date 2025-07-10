#pragma once
#include "common.hpp"
#include "opcode.hpp"
#include "result.hpp"
class Chunk;
class Value;

class VM {
public:
    Chunk* chunk;
    std::vector<Value> stack;
    std::unordered_map<std::string, Value> globals;

    void initVM();
    InterpretResult interpret(Chunk* chunk);
    InterpretResult interpret(const std::string source);
    InterpretResult run();
    Value peek(int distance);
    void runtimeError(std::string message, int codeIndex);
    void resetStack();
    Value pop();
    void push(Value value);
};