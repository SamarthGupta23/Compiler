#pragma once
#include "common.hpp"
#include <variant>
class Value;

class valueArray {
public:
    std::vector <Value> ValueVector;
    void initValueVector();
    void writeValueVector(Value value);
    void freeValueVector();
    Value getIndex(int index);
};