#include "common.hpp"
#include "chunk.hpp"
#include "debug.hpp"

void disassembleChunk(Chunk chunk, std::string name)
{
    std::cout << name << std::endl;
    std::vector <Opcode> instructions = chunk.getCode();
    for (int i = 0; i < instructions.size(); i++) {
        std::cout << std::setfill('0') << std::setw(4) << i << "    ";
        switch (instructions[i]) {
            case Opcode::OP_RETURN:
                std::cout << "OP_RETURN" << std::endl;
                break;  //break statements are needed after cases!!!
            case Opcode::OP_CONSTANT:
            {
                //constantIndex is the index of the constant stores in the chunk.constants array
                int constantIndex = static_cast<int>(instructions[i + 1]);
                std::cout << "OP_CONSTANT" << " " << chunk.constants.ValueVector[constantIndex].getDouble() << std::endl;
                i++; //as op_constant is a 2 byte OPCODE
                break;  // FIX: Add break statement!
            }
        }
    }
}