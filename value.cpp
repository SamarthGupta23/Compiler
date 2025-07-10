#include "value.hpp"

double Value::getDouble() {
    if (this->type == valueType::NUMBER) {
        return this->data.number;
    } else {
        throw std::runtime_error("NOT A DOUBLE");
    }
}

std::string Value::getString() {
    if (this->type == valueType::STRING) {
        return *this->data.string;
    } else {
        throw std::runtime_error("NOT A STRING");
    }
}

void Value::printValue() {
    if (this->type == valueType::NUMBER) {
        std::cout << this->data.number;
    }
    else if (this->type == valueType::BOOLEAN) {
        if (this->data.boolean) {
            std::cout << "true";
        } else {
            std::cout << "false";
        }
    }
    else if (this->type == valueType::NIL) {
        std::cout << "nil";
    }
    else if (this->type == valueType::STRING) {
        std::cout << *this->data.string;
    }
    else {
        throw std::runtime_error("UNKNOWN VALUE TYPE");
    }
}

void Value::negate() {
    if (this->type == valueType::NUMBER) {
        this->data.number = -this->data.number;
    }
}

bool Value::isFalsey() {
    return type == valueType::NIL || (type == valueType::BOOLEAN && !data.boolean);
}

// Fixed operator implementations - now as member functions
Value Value::operator+(const Value& other) const {
    if (this->type == valueType::NUMBER && other.type == valueType::NUMBER) {
        return Value(this->data.number + other.data.number);
    }
    throw std::runtime_error("Invalid operands for addition");
}

Value Value::operator-(const Value& other) const {
    if (this->type == valueType::NUMBER && other.type == valueType::NUMBER) {
        return Value(this->data.number - other.data.number);
    }
    throw std::runtime_error("Invalid operands for subtraction");
}

Value Value::operator*(const Value& other) const {
    if (this->type == valueType::NUMBER && other.type == valueType::NUMBER) {
        return Value(this->data.number * other.data.number);
    }
    throw std::runtime_error("Invalid operands for multiplication");
}

Value Value::operator/(const Value& other) const {
    if (this->type == valueType::NUMBER && other.type == valueType::NUMBER) {
        return Value(this->data.number / other.data.number);
    }
    throw std::runtime_error("Invalid operands for division");
}

void Value::setNil() {
    if (type == valueType::STRING) {
        delete data.string;
    }
    this->type = valueType::NIL;
    return;
}

void Value::printValue(std::ostream& os) {
    if (this->type == valueType::NUMBER) {
        os << this->data.number;
    }
    else if (this->type == valueType::BOOLEAN) {
        if (this->data.boolean) {
            os << "true";
        } else {
            os << "false";
        }
    }
    else if (this->type == valueType::NIL) {
        os << "nil";
    }
    else if (this->type == valueType::STRING) {
        os << *this->data.string;
    }
    else {
        throw std::runtime_error("UNKNOWN VALUE TYPE");
    }
}