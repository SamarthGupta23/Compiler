#include "scanner.hpp"
#include "token.hpp"

void Scanner::initScanner(std::string source) {
    source += '\0';
    this->line = 1;
    this->source = source;
    this->start = &(this->source[0]);
    this->current = &(this->source[0]);
    this->end = &(this->source[source.length()-1]);
}

Token Scanner::scanToken() {
    skipWhiteSpace();
    this->start = this->current;

    if (isAtEnd()) {
        return makeToken(TokenType::TOKEN_EOF);
    }

    char c = readAndAdvance();
    //first we check if its a number
    if (isAlpha(c)) {
        return identifier();
    }
    if (isDigit(c)) {
        return number();
    }

    switch (c) {
        case '(': return makeToken(TokenType::TOKEN_LEFT_PAREN);
        case ')': return makeToken(TokenType::TOKEN_RIGHT_PAREN);
        case '{': return makeToken(TokenType::TOKEN_LEFT_BRACE);
        case '}': return makeToken(TokenType::TOKEN_RIGHT_BRACE);
        case ';': return makeToken(TokenType::TOKEN_SEMICOLON);
        case ',': return makeToken(TokenType::TOKEN_COMMA);
        case '.': return makeToken(TokenType::TOKEN_DOT);
        case '-': return makeToken(TokenType::TOKEN_MINUS);
        case '+': return makeToken(TokenType::TOKEN_PLUS);
        case '/': return makeToken(TokenType::TOKEN_SLASH);
        case '*': return makeToken(TokenType::TOKEN_STAR);
        case '!':
            return makeToken(match('=') ? TokenType::TOKEN_BANG_EQUAL : TokenType::TOKEN_BANG);
        case '=':
            return makeToken(match('=') ? TokenType::TOKEN_EQUAL_EQUAL : TokenType::TOKEN_EQUAL);
        case '<':
            return makeToken(match('=') ? TokenType::TOKEN_LESS_EQUAL : TokenType::TOKEN_LESS);
        case '>':
            return makeToken(match('=') ? TokenType::TOKEN_GREATER_EQUAL : TokenType::TOKEN_GREATER);
        case '"':
            return string(); //make string token , but we need to set token lexeme as string ig?
    }

    return errorToken("Aint never seen this character before");
}

bool Scanner::isAtEnd() {
    return this->current >= this->end;
}

Token Scanner::makeToken(TokenType tokenType) {
    Token token;
    token.type = tokenType;
    token.lexeme = std::string(start, current);
    token.line = this->line;
    return token;
}

Token Scanner::errorToken(std::string message) {
    Token token;
    token.type = TokenType::TOKEN_ERROR;
    token.lexeme = message;
    token.line = this->line;
    return token;
}

char Scanner::readAndAdvance() {
    char currentChar = *(this->current);
    this->current++;
    return currentChar;
}

bool Scanner::match(char expected) {
    if (isAtEnd()) {
        return false;
    }
    if (*(this->current) == expected) {
        this->current++;
        return true;
    }
    return false;
}

void Scanner::skipWhiteSpace() {
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                readAndAdvance();
                break;
            case '\n':
                this->line++;
                readAndAdvance();
                break;
            default:
                return;
        }
    }
}

char Scanner::peek() {
    return *(this->current);
}

Token Scanner::string() {
    while (peek() != '"' && !isAtEnd()) {  //basically , stop if the string ends or the source file.
        if (peek() == '\n') {
            this->line++;
        }
        readAndAdvance();
    }

    //now if the source file ended , means the string wasn't closed ofc

    if (isAtEnd()) {
        return errorToken("close your goddamn strings bro PLEASE");
    }
    return makeToken(TokenType::TOKEN_STRING); //the token lexeme should have the actual string
}

bool Scanner::isDigit(char c) {
    if (c >= '0' && c <= '9') {
        return true;
    }
    return false;
}

Token Scanner::number() {
    while (isDigit(peek()) && !isAtEnd()) {
        readAndAdvance();
    }
        //for decimals
    if (peek() == '.' && isDigit(peekNext())) {
        readAndAdvance(); //to consume the decimal point

        while (isDigit(peek())) {
            readAndAdvance();
        }
    }
    return makeToken(TokenType::TOKEN_NUMBER);
}

char Scanner::peekNext() {
    if (isAtEnd()) {
        return '\0';
    }
    char * next = this->current;
    next++;
    return *next;
}

bool Scanner::isAlpha(char c) {
    if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') {
        return true;
    }
    if (c == '_') {
        return true;
    }
    return false;
}

Token Scanner::identifier() {
    while (isAlpha(peek()) || isDigit(peek())) {
        readAndAdvance();
    }
    return makeToken(identifierType());
}

TokenType Scanner::identifierType() {
    switch (*(this->start)) {
        case 'a' : return checkKeyword(1 , 2 , "nd" , TokenType::TOKEN_AND);
        case 'c' : return checkKeyword(1 , 4 , "lass" , TokenType::TOKEN_CLASS);
        case 'e' : return checkKeyword(1 , 3 , "lse" , TokenType::TOKEN_ELSE);
        case 'f':
            if (this->current - this->start > 1) {
                switch (this->start[1]) {
                    case 'a': return checkKeyword(2, 3, "lse", TokenType::TOKEN_FALSE);
                    case 'o': return checkKeyword(2, 1, "r", TokenType::TOKEN_FOR);
                    case 'u': return checkKeyword(2, 1, "n", TokenType::TOKEN_FUN);
                }
            }
            break;
        case 'i' : return checkKeyword(1 , 1 , "f" , TokenType::TOKEN_IF);
        case 'n' : return checkKeyword(1 , 2 , "il"   , TokenType::TOKEN_NIL);
        case 'o': return checkKeyword(1, 1, "r", TokenType::TOKEN_OR);
        case 'p': return checkKeyword(1, 4, "rint", TokenType::TOKEN_PRINT);
        case 'r': return checkKeyword(1, 5, "eturn", TokenType::TOKEN_RETURN);
        case 's': return checkKeyword(1, 4, "uper", TokenType::TOKEN_SUPER);
        case 't':
            if (this->current - this->start > 1) {
                switch (this->start[1]) {
                    case 'h': return checkKeyword(2, 2, "is", TokenType::TOKEN_THIS);
                    case 'r': return checkKeyword(2, 2, "ue", TokenType::TOKEN_TRUE);
                }
            }
            break;
        case 'v': return checkKeyword(1, 2, "ar", TokenType::TOKEN_VAR);
        case 'w': return checkKeyword(1, 4, "hile", TokenType::TOKEN_WHILE);
    }
    return TokenType::TOKEN_IDENTIFIER;
}

TokenType Scanner::checkKeyword(int start, int length,  std::string rest, TokenType type) {
    if (this->current - this->start == start + length &&
        std::string(this->start + start, length) == rest) {
        return type;
        }
    return TokenType::TOKEN_IDENTIFIER;
}