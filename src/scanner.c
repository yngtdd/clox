#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

typedef struct
{
    const char* start;
    const char* current;
    int line;
} Scanner;

Scanner scanner;

void scanner_init(const char* source)
{
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static bool is_at_end()
{
    return *scanner.current == '\0';
}

static Token token_make(TokenType type)
{
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;

    return token;
}

static Token token_error(const char* message)
{
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;

    return token;
}

/**
 * Get the next character
 */
static char advance()
{
    scanner.current++;
    return scanner.current[-1];
}

/**
 * Check on the current character
 */
static char peek() 
{
    return *scanner.current;
}

/**
 * Peek one step beyond the current character
 */
static char peek_next()
{
    if (is_at_end()) return '\0';
    // look one past the current character
    return scanner.current[1];
}

static void whitespace_skip()
{
    for (;;)
    {
        char c = peek();
        switch (c)
        {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;

            case '\n':
                scanner.line++;
                advance();
                break;

            case '/':
                if (peek_next() == '/')
                {
                    // Comments go to the end of the line
                    while (peek() != '\n' && !is_at_end()) advance();
                }
                else
                {
                    return;
                }
                break;

            default:
                return;
        }
    }
}


/**
 * Check whether a token is a paired token
 *
 * Certain tokens are made up of pairs of 
 * characters. When we find a token that potentially
 * has a pair, check to see if the next character is
 * the expected pair of that character. If it is not,
 * return false. If it is, return true. Our `scan_token`
 * will check each possible case.
 */
static bool pair(char expected)
{
    if (is_at_end()) return false;
    if (*scanner.current != expected) return false;

    scanner.current++;
    return true;
}

Token token_scan()
{
    whitespace_skip();
    scanner.start = scanner.current;

    if (is_at_end()) return token_make(TOKEN_EOF);

    char c = advance();

    switch (c)
    {
        case '(': return token_make(TOKEN_LEFT_PAREN);
        case ')': return token_make(TOKEN_RIGHT_PAREN);
        case '{': return token_make(TOKEN_LEFT_BRACE);
        case '}': return token_make(TOKEN_RIGHT_BRACE);
        case ';': return token_make(TOKEN_SEMICOLON);
        case ',': return token_make(TOKEN_COMMA);
        case '.': return token_make(TOKEN_DOT);
        case '-': return token_make(TOKEN_MINUS);
        case '+': return token_make(TOKEN_PLUS);
        case '/': return token_make(TOKEN_SLASH);
        case '*': return token_make(TOKEN_STAR);
        // Potential two character tokens
        case '!': 
            return token_make(pair('=') ? TOKEN_BANG_EQUAL: TOKEN_BANG);
        case '=':
            return token_make(pair('=') ? TOKEN_EQUAL_EQUAL: TOKEN_EQUAL);
        case '<':
            return token_make(pair('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
            return token_make(pair('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    }

    return token_error("Unexpected character.");
}
