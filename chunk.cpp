#include "chunk.hpp"

void Chunk::initChunk()
{
    this->code = {};
    this->constants.initValueVector();
}

void Chunk::writeChunk(Opcode byte, int line)
{
    this->lines.push_back(line);
    this->code.push_back(byte);
}

void Chunk::freeChunk()
{
    this->code.clear();
    this->constants.freeValueVector();
}

int Chunk::addConstant(Value value)
{
    this->constants.ValueVector.push_back(value);
    return constants.ValueVector.size() - 1;
}

std::vector<Opcode> Chunk::getCode()
{
    return this->code;
}

valueArray Chunk::getValueArray()
{
    return this->constants;
}

std::vector<int> Chunk::getLines()
{
    return this->lines;
}
