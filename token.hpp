#pragma once
#include "common.hpp"
#include "tokentype.hpp"

class Token {
public:
    TokenType type;
    std::string lexeme;
    int line;

    // Default constructor
    Token() : type(TokenType::TOKEN_ERROR), lexeme(""), line(0) {}

    // Constructor with parameters
    Token(TokenType t, std::string l, int ln) : type(t), lexeme(l), line(ln) {}
};