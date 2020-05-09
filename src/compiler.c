#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "compiler.h"
#include "chunk.h"
#include "value.h"
#include "scanner.h"

typedef struct
{
    Token current;
    Token previous;
    bool had_error;
    bool panic_mode;
} Parser;

/**
 * Lox Precedence
 *
 * This is ordered from lowest to highest
 */
typedef enum
{
    PREC_NONE,
    PREC_ASSIGNMENT, // =
    PREC_OR,         // or
    PREC_AND,        // and
    PREC_EQUALITY,   // == !=
    PREC_COMPARISON, // < > <= >=
    PREC_TERM,       // + -
    PREC_FACTOR,     // * /
    PREC_UNARY,      // ! -
    PREC_CALL,       // . ()
    PREC_PRIMARY,
} Precedence;

/**
 * Function pointer type
 *
 * Function pointer syntanx is a little wild.
 * This wraps it up in a type to prevent me 
 * from making syntax mistakes.
 */
typedef void (*ParseFn)();

typedef struct
{
    ParseFn prefix;
    ParseFn infix;
    Precedence precedece;
} ParseRule;

Parser parser;
Chunk* chunk_compiling;

static Chunk* chunk_current()
{
    return chunk_compiling;
}

static void error_at(Token* token, const char* message)
{
    if (parser.panic_mode) return;
    parser.panic_mode = true;

    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF)
    {
        fprintf(stderr, " at end");
    }
    else if (token->type == TOKEN_ERROR)
    {
        // Nothing
    }
    else
    {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.had_error = true;
}

static void error_at_current(const char* message)
{
    error_at(&parser.previous, message);
}

static void error(const char* message)
{
    error_at(&parser.previous, message);
}


static void advance()
{
    parser.previous = parser.current;

    for (;;)
    {
        parser.current = token_scan();
        if (parser.current.type != TOKEN_ERROR) break;

        error_at_current(parser.current.start);
    }
}

static void consume(TokenType type, const char* message)
{
    if (parser.current.type == type)
    {
        advance();
        return;
    }

    error_at_current(message);
}

static void byte_emit(uint8_t byte)
{
    chunk_write(chunk_current(), byte, parser.previous.line);
}

static void bytes_emit(uint8_t byte1, uint8_t byte2)
{
    byte_emit(byte1);
    byte_emit(byte2);
}

static void return_emit()
{
    byte_emit(OP_RETURN);
}

static void compiler_end()
{
    return_emit();
}

static void expression();
static ParseRule* rule_get(TokenType type);
static void parse_precedence(Precedence precedece);

static void binary()
{
    // Remember the operator
    TokenType operator_type = parser.previous.type;

    // Compile the right operand
    ParseRule* rule = rule_get(operator_type);
    parse_precedence((Precedence)(rule->precedece + 1));

    // Emit the operator instruction.
    switch (operator_type)
    {
        case TOKEN_PLUS:   byte_emit(OP_ADD); break;
        case TOKEN_MINUS:  byte_emit(OP_MINUS); break;
        case TOKEN_STAR:   byte_emit(OP_MULTIPLY); break;
        case TOKEN_SLASH:  byte_emit(OP_DIVIDE); break;
        default:
            return; // Unreachable
    }

}

static void grouping()
{
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static uint8_t constant_make(Value value)
{
    // Append constant to the chunk's constant table, and get it's index
    int constant = chunk_constant_add(chunk_current(), value);

    if (constant > UINT8_MAX)
    {
        error("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)constant;
}

static void constant_emit(Value value)
{
    bytes_emit(OP_CONSTANT, constant_make(value));
}

static void number()
{
    double value = strtod(parser.previous.start, NULL);
    constant_emit(value);
}

static void unary()
{
    TokenType operator_type = parser.previous.type;

    // Compile the operand.
    expression(PREC_UNARY);

    // Emit the operator instruction
    switch (operator_type)
    {
        case TOKEN_MINUS: emit_byte(OP_NEGATE); break;
        default:
            return; // unreachable
    }
}

ParseRule rules[] = 
{
    { grouping, NULL,   PREC_NONE },   // TOKEN_LEFT_PAREN
    { NULL,     NULL,   PREC_NONE },   // TOKEN_RIGHT_PAREN
    { NULL,     NULL,   PREC_NONE },   // TOKEN_LEFT_BRACE
    { NULL,     NULL,   PREC_NONE },   // TOKEN_RIGHT_BRACE
    { NULL,     NULL,   PREC_NONE },   // TOKEN_COMMA
    { NULL,     NULL,   PREC_NONE },   // TOKEN_DOT
    { unary,    binary, PREC_TERM },   // TOKEN_MINUS
    { NULL,     binary, PREC_TERM },   // TOKEN_PLUS
    { NULL,     NULL,   PREC_NONE },   // TOKEN_SEMICOLON
    { NULL,     binary, PREC_FACTOR }, // TOKEN_SLASH
    { NULL,     binary, PREC_FACTOR }, // TOKEN_STAR
    { NULL,     NULL,   PREC_NONE },   // TOKEN_BANG
    { NULL,     NULL,   PREC_NONE },   // TOKEN_BANG_EQUAL
    { NULL      NULL,   PREC_NONE },   // TOKEN_EQUAL
    { NULL,     NULL,   PREC_NONE },   // TOKEN_GREATER
    { NULL,     NULL,   PREC_NONE },   // TOKEN_GREATER_EQUAL
    { NULL,     NULL,   PREC_NONE },   // TOKEN_LESS
    { NULL,     NULL,   PREC_NONE },   // TOKEN_LESS_EQUAL
    { NULL,     NULL,   PREC_NONE },   // TOKEN_IDENTIFIER
    { NULL,     NULL,   PREC_NONE },   // TOKEN_STRING
    { number,   NULL,   PREC_NONE },   // TOKEN_NUMBER
    { NULL,     NULL,   PREC_NONE },   // TOKEN_AND
    { NULL,     NULL,   PREC_NONE },   // TOKEN_CLASS
    { NULL,     NULL,   PREC_NONE },   // TOKEN_ELSE
    { NULL,     NULL,   PREC_NONE },   // TOKEN_FALSE
    { NULL,     NULL,   PREC_NONE },   // TOKEN_FOR
    { NULL,     NULL,   PREC_NONE },   // TOKEN_FN
    { NULL,     NULL,   PREC_NONE },   // TOKEN_IF
    { NULL,     NULL,   PREC_NONE },   // TOKEN_NIL
    { NULL,     NULL,   PREC_NONE },   // TOKEN_OR
    { NULL,     NULL,   PREC_NONE },   // TOKEN_PRINT
    { NULL,     NULL,   PREC_NONE },   // TOKEN_RETURN
    { NULL,     NULL,   PREC_NONE },   // TOKEN_SUPER
    { NULL,     NULL,   PREC_NONE },   // TOKEN_THIS
    { NULL,     NULL,   PREC_NONE },   // TOKEN_TRUE
    { NULL,     NULL,   PREC_NONE },   // TOKEN_LET
    { NULL,     NULL,   PREC_NONE },   // TOKEN_WHILEJ
    { NULL,     NULL,   PREC_NONE },   // TOKEN_ERROR
    { NULL,     NULL,   PREC_NONE },   // TOKEN_EOF

};

static void parse_precedence(Precedence precedece)
{
    // What goes here?
}

static ParseRule* rule_get(TokenType type)
{
    return &rules[type];
}

static void expression()
{
    parse_precedence(PREC_ASSIGNMENT);
}

bool compile(const char* source, Chunk* chunk)
{
    scanner_init(source);
    chunk_compiling = chunk;

    parser.had_error = false;
    parser.panic_mode = false;

    advance();
    expression();
    consume(TOKEN_EOF, "Expect end of expression.");
    compiler_end();

    return !parser.had_error;
}


