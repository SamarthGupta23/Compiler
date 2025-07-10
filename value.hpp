#pragma once
#include "common.hpp"
#include <variant>

class Value {
public:
    valueType type;
    union {
        double number;
        bool boolean;
        std::string* string;  // For identifiers
    } data;

    Value(double input) {     //constructor for double datatype
        type = valueType::NUMBER;
        data.number = input;
    }

    Value(bool input) {
        type = valueType::BOOLEAN;
        data.boolean = input;
    }

    Value(std::string input) {  // Constructor for string identifiers
        type = valueType::STRING;
        data.string = new std::string(input);
    }

    // Copy constructor
    Value(const Value& other) {
        type = other.type;
        if (type == valueType::STRING) {
            data.string = new std::string(*other.data.string);
        } else {
            data = other.data;
        }
    }
    Value() {
        type = valueType::NIL;
    }
    // Assignment operator
    Value& operator=(const Value& other) {
        if (this != &other) {
            if (type == valueType::STRING) {
                delete data.string;
            }
            type = other.type;
            if (type == valueType::STRING) {
                data.string = new std::string(*other.data.string);
            } else {
                data = other.data;
            }
        }
        return *this;
    }

    ~Value() {
        if (type == valueType::STRING) {
            delete data.string;
        }
    }

    void setNil();
    void printValue();
    void negate();
    bool isFalsey();
    void printValue(std::ostream& os); // for insides , to see the opcodes and shit

    Value operator+(const Value& other) const;
    Value operator-(const Value& other) const;
    Value operator*(const Value& other) const;
    Value operator/(const Value& other) const;

    double getDouble();
    std::string getString();
};