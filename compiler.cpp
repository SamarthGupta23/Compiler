#include "compiler.hpp"
#include "scanner.hpp"
#include "token.hpp"
#include "tokentype.hpp"
#include "chunk.hpp"
#include "parser.hpp"
#include "value.hpp"
#include <cstdlib>

// Globals for the compiler state
Scanner scanner;
Parser parser;
bool hadError;
bool panicMode;
Chunk* currentChunk;

void advance();
void error(std::string message);
void consume(TokenType type, std::string message);
void throwError(Token* token, std::string message);
void emitByte(Opcode opcode);
void endCompiler();
void emitBytes(Opcode byte1, Opcode byte2);
void expression();
void statement();
void declaration();
void varDeclaration();
void printStatement();
void expressionStatement();
void parsePrecedence(Precedence precedence);
ParseRule* getRule(TokenType type);
void number(bool canAssign);
void grouping(bool canAssign);
void unary(bool canAssign);
void binary(bool canAssign);
void literal(bool canAssign);
void variable(bool canAssign);
void emitConstant(Value value);
int makeConstant(Value value);
int identifierConstant(Token* name);
void namedVariable(Token name, bool canAssign);
bool match(TokenType type);
int parseVariable(std::string errorMessage);
void defineVariable(int global);
void synchronize();

ParseRule rules[] = {
    [static_cast<int>(TokenType::TOKEN_LEFT_PAREN)]    = {grouping, NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_RIGHT_PAREN)]   = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_LEFT_BRACE)]    = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_RIGHT_BRACE)]   = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_COMMA)]         = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_DOT)]           = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_MINUS)]         = {unary,    binary, Precedence::PREC_TERM},
    [static_cast<int>(TokenType::TOKEN_PLUS)]          = {NULL,     binary, Precedence::PREC_TERM},
    [static_cast<int>(TokenType::TOKEN_SEMICOLON)]     = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_SLASH)]         = {NULL,     binary, Precedence::PREC_FACTOR},
    [static_cast<int>(TokenType::TOKEN_STAR)]          = {NULL,     binary, Precedence::PREC_FACTOR},
    [static_cast<int>(TokenType::TOKEN_BANG)]          = {unary,    NULL,   Precedence::PREC_UNARY},
    [static_cast<int>(TokenType::TOKEN_BANG_EQUAL)]    = {NULL,     binary, Precedence::PREC_EQUALITY},
    [static_cast<int>(TokenType::TOKEN_EQUAL)]         = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_EQUAL_EQUAL)]   = {NULL,     binary, Precedence::PREC_EQUALITY},
    [static_cast<int>(TokenType::TOKEN_GREATER)]       = {NULL,     binary, Precedence::PREC_COMPARISON},
    [static_cast<int>(TokenType::TOKEN_GREATER_EQUAL)] = {NULL,     binary, Precedence::PREC_COMPARISON},
    [static_cast<int>(TokenType::TOKEN_LESS)]          = {NULL,     binary, Precedence::PREC_COMPARISON},
    [static_cast<int>(TokenType::TOKEN_LESS_EQUAL)]    = {NULL,     binary, Precedence::PREC_COMPARISON},
    [static_cast<int>(TokenType::TOKEN_IDENTIFIER)]    = {variable, NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_STRING)]        = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_NUMBER)]        = {number,   NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_AND)]           = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_CLASS)]         = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_ELSE)]          = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_FALSE)]         = {literal,  NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_FOR)]           = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_FUN)]           = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_IF)]            = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_NIL)]           = {literal,  NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_OR)]            = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_PRINT)]         = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_RETURN)]        = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_SUPER)]         = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_THIS)]          = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_TRUE)]          = {literal,  NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_VAR)]           = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_WHILE)]         = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_ERROR)]         = {NULL,     NULL,   Precedence::PREC_NONE},
    [static_cast<int>(TokenType::TOKEN_EOF)]           = {NULL,     NULL,   Precedence::PREC_NONE},
};

bool compile(std::string source, Chunk* chunk) {
    scanner.initScanner(source);
    currentChunk = chunk;
    hadError = false;
    panicMode = false;
    advance();

    while (!match(TokenType::TOKEN_EOF)) {
        declaration();
    }

    endCompiler();
    return !hadError;
}

void advance() {
    parser.previous = parser.current;
    while (true) {
        parser.current = scanner.scanToken();
        if (parser.current.type != TokenType::TOKEN_ERROR) break;
        error(parser.current.lexeme);
    }
}

void error(std::string message) {
    if (panicMode) return;
    panicMode = true;
    throwError(&parser.current, message);
}

void throwError(Token* token, std::string message) {
    std::cerr << "Line " << token->line << ": " << message << std::endl;
    hadError = true;
}

void consume(TokenType type, std::string message) {
    if (parser.current.type == type) {
        advance();
        return;
    }
    error(message);
}

bool match(TokenType type) {
    if (parser.current.type == type) {
        advance();
        return true;
    }
    return false;
}

void declaration() {
    if (match(TokenType::TOKEN_VAR)) {
        varDeclaration();
    } else {
        statement();
    }
    if (panicMode) synchronize();
}

void varDeclaration() {
    int global = parseVariable("Expect variable name.");

    if (match(TokenType::TOKEN_EQUAL)) {
        expression();
    } else {
        emitByte(Opcode::OP_NIL);
    }
    consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after variable declaration.");
    defineVariable(global);
}

int parseVariable(std::string errorMessage) {
    consume(TokenType::TOKEN_IDENTIFIER, errorMessage);
    return identifierConstant(&parser.previous);
}

void defineVariable(int global) {
    emitBytes(Opcode::OP_DEFINE_GLOBAL, static_cast<Opcode>(global));
}

void synchronize() {
    panicMode = false;
    while (parser.current.type != TokenType::TOKEN_EOF) {
        if (parser.previous.type == TokenType::TOKEN_SEMICOLON) return;
        switch (parser.current.type) {
            case TokenType::TOKEN_CLASS:
            case TokenType::TOKEN_FUN:
            case TokenType::TOKEN_VAR:
            case TokenType::TOKEN_FOR:
            case TokenType::TOKEN_IF:
            case TokenType::TOKEN_WHILE:
            case TokenType::TOKEN_PRINT:
            case TokenType::TOKEN_RETURN:
                return;
            default:
                break;
        }
        advance();
    }
}

void statement() {
    if (match(TokenType::TOKEN_PRINT)) {
        printStatement();
    } else {
        expressionStatement();
    }
}

void printStatement() {
    expression();
    consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after value.");
    emitByte(Opcode::OP_PRINT);
}

void expressionStatement() {
    expression();
    consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after expression.");
    emitByte(Opcode::OP_POP);
}

void emitByte(Opcode opcode) {
    currentChunk->writeChunk(opcode, parser.current.line);
}

void endCompiler() {
    emitByte(Opcode::OP_RETURN);
}

void emitBytes(Opcode byte1, Opcode byte2) {
    emitByte(byte1);
    emitByte(byte2);
}

void expression() {
    parsePrecedence(Precedence::PREC_ASSIGNMENT);
}

void parsePrecedence(Precedence precedence) {
    advance();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if (prefixRule == NULL) {
        error("Expect expression.");
        return;
    }
    bool canAssign = precedence <= Precedence::PREC_ASSIGNMENT;
    prefixRule(canAssign);

    while (precedence <= getRule(parser.current.type)->precedence) {
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule(canAssign);
    }

    if (canAssign && match(TokenType::TOKEN_EQUAL)) {
        error("Invalid assignment target.");
    }
}

ParseRule* getRule(TokenType type) {
    return &rules[static_cast<int>(type)];
}

void number(bool) {
    double value = std::stod(parser.previous.lexeme);
    emitConstant(Value(value));
}

void grouping(bool) {
    expression();
    consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

void unary(bool) {
    TokenType operatorType = parser.previous.type;
    parsePrecedence(Precedence::PREC_UNARY);

    switch (operatorType) {
        case TokenType::TOKEN_MINUS:
            emitByte(Opcode::OP_NEGATE);
            break;
        case TokenType::TOKEN_BANG:
            emitByte(Opcode::OP_NOT);
            break;
        default:
            return;
    }
}

void binary(bool) {
    TokenType operatorType = parser.previous.type;
    ParseRule* rule = getRule(operatorType);
    parsePrecedence(static_cast<Precedence>(static_cast<int>(rule->precedence) + 1));

    switch (operatorType) {
        case TokenType::TOKEN_PLUS:
            emitByte(Opcode::OP_ADD);
            break;
        case TokenType::TOKEN_MINUS:
            emitByte(Opcode::OP_SUBTRACT);
            break;
        case TokenType::TOKEN_STAR:
            emitByte(Opcode::OP_MULTIPLY);
            break;
        case TokenType::TOKEN_SLASH:
            emitByte(Opcode::OP_DIVIDE);
            break;
        case TokenType::TOKEN_GREATER:
            emitByte(Opcode::OP_GREATER);
            break;
        case TokenType::TOKEN_LESS:
            emitByte(Opcode::OP_LESSER);
            break;
        case TokenType::TOKEN_EQUAL_EQUAL:
            emitByte(Opcode::OP_EQUAL);
            break;
        case TokenType::TOKEN_GREATER_EQUAL:
            emitBytes(Opcode::OP_LESSER , Opcode::OP_NOT);
            break;
        case TokenType::TOKEN_LESS_EQUAL:
            emitBytes(Opcode::OP_GREATER, Opcode::OP_NOT);
            break;
        case TokenType::TOKEN_BANG_EQUAL:
            emitBytes(Opcode::OP_EQUAL , Opcode::OP_NOT);
            break;
        default:
            return;
    }
}

void literal(bool) {
    switch (parser.previous.type) {
        case TokenType::TOKEN_FALSE:
            emitByte(Opcode::OP_FALSE);
            break;
        case TokenType::TOKEN_TRUE:
            emitByte(Opcode::OP_TRUE);
            break;
        case TokenType::TOKEN_NIL:
            emitByte(Opcode::OP_NIL);
            break;
        default:
            return;
    }
}

// ------ IDENTIFIER AND VARIABLE HANDLING ------

void variable(bool canAssign) {
    namedVariable(parser.previous, canAssign);
}

void namedVariable(Token name, bool canAssign) {
    int arg = identifierConstant(&name);

    if (canAssign && match(TokenType::TOKEN_EQUAL)) {
        expression();
        emitBytes(Opcode::OP_SET_GLOBAL, static_cast<Opcode>(arg));
    } else {
        emitBytes(Opcode::OP_GET_GLOBAL, static_cast<Opcode>(arg));
    }
}

int identifierConstant(Token* name) {
    return makeConstant(Value(name->lexeme));
}

void emitConstant(Value value) {
    emitBytes(Opcode::OP_CONSTANT, static_cast<Opcode>(makeConstant(value)));
}

int makeConstant(Value value) {
    int constant = currentChunk->addConstant(value);
    if (constant > 255) {
        error("Too many constants in one chunk.");
        return 0;
    }
    return constant;
}