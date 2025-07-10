#pragma once
#include "common.hpp"
#include "tokentype.hpp"

class Token;

class Scanner {
public:
    char * start;
    char * current;
    char * end;
    int line;
    std::string source;

    void initScanner(std::string source);
    Token scanToken();
    Token makeToken(TokenType tokenType);
    Token errorToken(std::string message);
    char readAndAdvance();
    bool match(char expected);
    void skipWhiteSpace();
    bool isAtEnd();
    char peek();
    Token string();
    bool isDigit(char c);
    Token number();
    char peekNext();
    bool isAlpha(char c);
    Token identifier();
    TokenType identifierType();
    TokenType checkKeyword(int start  , int length , std::string rest , TokenType type); //start is the point where we want to start as some identifiers can start with the same letter (eg -> false , for , fun)
};