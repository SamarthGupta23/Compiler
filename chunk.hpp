#pragma once
#include "common.hpp"
#include "value.hpp"
#include "opcode.hpp"
#include "valuearray.hpp"

class Chunk {
public:
    std::vector <Opcode> code; //vector of bytecode
    std::vector <int> lines; //we store lines in this , line number of where the opcode was passed into it
    valueArray constants; //vector of constants

    void initChunk();
    void writeChunk(Opcode byte, int line);
    void freeChunk();
    int addConstant(Value value);  // FIX: Return int, not Opcode
    //getters

    std::vector <Opcode> getCode();
    valueArray getValueArray();
    std::vector <int> getLines();
};