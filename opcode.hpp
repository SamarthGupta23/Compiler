#pragma once

enum class Opcode : int {
    OP_RETURN,    //1 byte OPCODE
    OP_NEGATE,    //make the number negative
    OP_CONSTANT, //2 byte OPCODE
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_NOT,
    OP_EQUAL, //maps to == not =  , as = is a statement
    OP_GREATER,
    OP_LESSER,
    // New opcodes for variables
    OP_DEFINE_GLOBAL,  // Define a global variable
    OP_GET_GLOBAL,     // Get a global variable value
    OP_SET_GLOBAL,     // Set a global variable value
    OP_POP,            // Pop value from stack
    OP_PRINT,          // Print statement
};
