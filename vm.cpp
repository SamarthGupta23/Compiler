#include "vm.hpp"
#include "value.hpp"
#include "valuearray.hpp"
#include "chunk.hpp"
#include "opcode.hpp"
#include "compiler.hpp"

//for writing in ot insides , to show the opcodes
std::string getString(Opcode opcode) {
    switch (opcode) {
        case Opcode::OP_RETURN:        return "OP_RETURN";
        case Opcode::OP_NEGATE:        return "OP_NEGATE";
        case Opcode::OP_CONSTANT:      return "OP_CONSTANT";
        case Opcode::OP_ADD:           return "OP_ADD";
        case Opcode::OP_SUBTRACT:      return "OP_SUBTRACT";
        case Opcode::OP_MULTIPLY:      return "OP_MULTIPLY";
        case Opcode::OP_DIVIDE:        return "OP_DIVIDE";
        case Opcode::OP_NIL:           return "OP_NIL";
        case Opcode::OP_TRUE:          return "OP_TRUE";
        case Opcode::OP_FALSE:         return "OP_FALSE";
        case Opcode::OP_NOT:           return "OP_NOT";
        case Opcode::OP_EQUAL:         return "OP_EQUAL";
        case Opcode::OP_GREATER:       return "OP_GREATER";
        case Opcode::OP_LESSER:        return "OP_LESSER";
        case Opcode::OP_DEFINE_GLOBAL: return "OP_DEFINE_GLOBAL";
        case Opcode::OP_GET_GLOBAL:    return "OP_GET_GLOBAL";
        case Opcode::OP_SET_GLOBAL:    return "OP_SET_GLOBAL";
        case Opcode::OP_POP:           return "OP_POP";
        case Opcode::OP_PRINT:         return "OP_PRINT";
        default:                       return "UNKNOWN_OPCODE";
    }
}

Value VM::peek(int distance) {
    return this->stack[stack.size() - 1 - distance];
}

InterpretResult VM::interpret(Chunk* chunk) {
    this->chunk = chunk;
    return run();
}

InterpretResult VM::run() {
    for (int i = 0; i < this->chunk->code.size(); i++) {
        switch (this->chunk->code[i]) {
            case Opcode::OP_NEGATE:
                if (peek(0).type != valueType::NUMBER) {
                    this->runtimeError("You do know only numbers support '-' right?", i);
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                this->stack[stack.size() - 1].negate();
                break;
            case Opcode::OP_ADD: {
                Value addSecond = this->pop();
                Value addFirst = this->pop();
                if (addFirst.type != valueType::NUMBER || addSecond.type != valueType::NUMBER) {
                    this->runtimeError("Invalid operation for given operands", i);
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                push(addFirst + addSecond);
                break;
            }
            case Opcode::OP_SUBTRACT: {
                Value subtractSecond = this->pop();
                Value subtractFirst = this->pop();
                if (subtractFirst.type != valueType::NUMBER || subtractSecond.type != valueType::NUMBER) {
                    this->runtimeError("Invalid operation for given operands", i);
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                push(subtractFirst - subtractSecond);
                break;
            }
            case Opcode::OP_MULTIPLY: {
                Value multiplySecond = this->pop();
                Value multiplyFirst = this->pop();
                if (multiplyFirst.type != valueType::NUMBER || multiplySecond.type != valueType::NUMBER) {
                    this->runtimeError("Invalid operation for given operands", i);
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                push(multiplyFirst * multiplySecond);
                break;
            }
            case Opcode::OP_DIVIDE: {
                Value divideSecond = this->pop();
                Value divideFirst = this->pop();
                if (divideFirst.type != valueType::NUMBER || divideSecond.type != valueType::NUMBER) {
                    this->runtimeError("Invalid operation for given operands", i);
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                push(divideFirst / divideSecond);
                break;
            }
            case Opcode::OP_RETURN: {
                return InterpretResult::INTERPRET_OK;
            }
            case Opcode::OP_CONSTANT: {
                push(this->chunk->constants.ValueVector[static_cast<int>(this->chunk->code[i + 1])]);
                i++;
                break;
            }
            case Opcode::OP_FALSE: {
                push(Value(false));
                break;
            }
            case Opcode::OP_TRUE: {
                push(Value(true));
                break;
            }
            case Opcode::OP_NIL: {
                Value value(false);
                value.setNil();
                push(value);
                break;
            }
            case Opcode::OP_NOT: {
                Value value = pop();
                push(Value(value.isFalsey()));
                break;
            }
            case Opcode::OP_EQUAL: {
                Value first = this->pop();
                Value second = this->pop();
                if (first.type != second.type) {
                    push(Value(false));
                } else {
                    if (first.type == valueType::NUMBER) {
                        push(Value(first.data.number == second.data.number));
                    } else if (first.type == valueType::BOOLEAN) {
                        push(Value(first.data.boolean == second.data.boolean));
                    } else if (first.type == valueType::NIL) {
                        push(Value(true));
                    } else {
                        push(Value(false));
                    }
                }
                break;
            }
            case Opcode::OP_GREATER: {
                Value second = this->pop();
                Value first = this->pop();
                if (first.type != valueType::NUMBER || second.type != valueType::NUMBER) {
                    this->runtimeError("Invalid operation for given operands", i);
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                push(Value(first.data.number > second.data.number));
                break;
            }
            case Opcode::OP_LESSER: {
                Value second = this->pop();
                Value first = this->pop();
                if (first.type != valueType::NUMBER || second.type != valueType::NUMBER) {
                    this->runtimeError("Invalid operation for given operands", i);
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                push(Value(first.data.number < second.data.number));
                break;
            }
            case Opcode::OP_PRINT: {
                pop().printValue();
                std::cout << std::endl;
                break;
            }
            case Opcode::OP_POP: {
                pop();
                break;
            }
            case Opcode::OP_DEFINE_GLOBAL: {
                std::string name = this->chunk->constants.ValueVector[static_cast<int>(this->chunk->code[i + 1])].getString();
                globals[name] = peek(0);
                pop();
                i++;
                break;
            }
            case Opcode::OP_GET_GLOBAL: {
                std::string name = this->chunk->constants.ValueVector[static_cast<int>(this->chunk->code[i + 1])].getString();
                if (globals.find(name) == globals.end()) {
                    this->runtimeError("Undefined variable '" + name + "'", i);
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                push(globals[name]);
                i++;
                break;
            }
            case Opcode::OP_SET_GLOBAL: {
                std::string name = this->chunk->constants.ValueVector[static_cast<int>(this->chunk->code[i + 1])].getString();
                if (globals.find(name) == globals.end()) {
                    this->runtimeError("Undefined variable '" + name + "'", i);
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                globals[name] = peek(0);
                i++;
                break;
            }
        }
    }
    return InterpretResult::INTERPRET_OK;
}

void VM::initVM() {
    this->stack = {};
    this->globals.clear();
}

Value VM::pop() {
    Value result = this->stack.back();
    this->stack.pop_back();
    return result;
}

void VM::push(Value value) {
    this->stack.push_back(value);
}

InterpretResult VM::interpret(const std::string source) {
    Chunk chunk;
    chunk.initChunk();

    if (!compile(source, &chunk)) {
        // Dump opcode/constant info on compile error too
        std::ofstream out("C:\\Users\\samar\\CLionProjects\\cppcompiler\\insides.lol");
        out << "Opcode Array:\n";
        auto code = chunk.getCode();
        for (size_t i = 0; i < code.size(); ++i) {
            out << i << ": " << getString(code[i]);
            if (code[i] == Opcode::OP_CONSTANT || code[i] == Opcode::OP_DEFINE_GLOBAL ||
                code[i] == Opcode::OP_GET_GLOBAL || code[i] == Opcode::OP_SET_GLOBAL) {
                out << " [" << static_cast<int>(code[i+1]) << "]";
                ++i;
                }
            out << "\n";
        }

        out.close();
        chunk.freeChunk();
        return InterpretResult::INTERPRET_COMPILE_ERROR;
    }

    // Dump after successful compile
    std::ofstream out("C:\\Users\\samar\\CLionProjects\\cppcompiler\\insides.lol");
    out << "Opcode Array:\n";
    auto code = chunk.getCode();
    for (size_t i = 0; i < code.size(); ++i) {
        out << i << ": " << getString(code[i]);
        if (code[i] == Opcode::OP_CONSTANT || code[i] == Opcode::OP_DEFINE_GLOBAL ||
            code[i] == Opcode::OP_GET_GLOBAL || code[i] == Opcode::OP_SET_GLOBAL) {
            out << " [" << static_cast<int>(code[i+1]) << "]";
            ++i;
            }
        out << "\n";
    }
    out << "Constants:\n";
    auto constants = chunk.getValueArray().ValueVector;

    out.close();

    this->chunk = &chunk;
    InterpretResult result = run();

    chunk.freeChunk();
    return result;
}
void VM::runtimeError(std::string message, int codeIndex) {
    std::cout << "Runtime Error: " << message << " at line " << this->chunk->lines[codeIndex] << std::endl;
    resetStack();
}

void VM::resetStack() {
    this->stack.clear();
}