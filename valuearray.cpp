#include "value.hpp"
#include "valuearray.hpp"

void valueArray::initValueVector()
{
    this->ValueVector = {};
}

void valueArray::writeValueVector(Value value)
{
    this->ValueVector.push_back(value);
}

void valueArray::freeValueVector()
{
    this->ValueVector.clear();
}

Value valueArray::getIndex(int index) {
    if (index >= 0 && index < this->ValueVector.size()) {
        return this->ValueVector[index];
    }
    else {
        Value value;
        value.setNil();
        return value;
    }
}