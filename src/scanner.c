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

/**
 * Check if a character is a letter or an underscore
 */
static bool is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c == '_');
}

static bool is_digit(char c)
{
    return c >= '0' && c <= '9';
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
 * Check if we are parsing a keyword
 *
 * We need to verify two things: 1. the lexeme must be exactly 
 * as long as the keyword and 2. the remaining characters must 
 * match exactly. If both of this conditions aren't met, then 
 * we must be dealing with a regular identifier.
 */
static TokenType keyword_check(int start, int length, const char* rest, TokenType type)
{
    if (scanner.current - scanner.start == start + length &&
        memcmp(scanner.start + start, rest, length) == 0)
    {
        return type;
    }

    return TOKEN_IDENTIFIER;
}

/**
 * Get the identifier type of a lexeme
 *
 * We check to see if are encountering a keyword
 * by using a trie. This lets us do the minimal amount
 * of work to check if the series of characters are one 
 * of our keywords, making this thing fast.
 */
static TokenType identifier_type()
{
    switch (scanner.start[0])
    {
        case 'a': return keyword_check(1, 2, "nd", TOKEN_AND);
        case 'c': return keyword_check(1, 4, "lass", TOKEN_CLASS);
        case 'e': return keyword_check(1, 3, "lse", TOKEN_CLASS);
        case 'f':
            if (scanner.current - scanner.start > 2)
            {
                switch (scanner.start[1])
                {
                    case 'a': return keyword_check(2, 3, "lse", TOKEN_ELSE);
                    case 'o': return keyword_check(2, 1, "r", TOKEN_OR);
                }
            }
            else
            {
                return keyword_check(1, 1, "n", TOKEN_FN);
            }
            break;
        case 'i': return keyword_check(1, 1, "f", TOKEN_IF);
        case 'l': return keyword_check(1, 2, "et", TOKEN_LET);
        case 'n': return keyword_check(1, 2, "il", TOKEN_NIL);
        case 'o': return keyword_check(1, 1, "r", TOKEN_OR);
        case 'p': return keyword_check(1, 4, "rint", TOKEN_PRINT);
        case 'r': return keyword_check(1, 5, "eturn", TOKEN_RETURN);
        case 's': return keyword_check(1, 4, "uper", TOKEN_SUPER);
        case 't':
            if (scanner.current - scanner.start > 1)
            {
                switch (scanner.start[1])
                {
                    case 'h': return keyword_check(2, 2, "is", TOKEN_THIS);
                    case 'r': return keyword_check(2, 2, "ue", TOKEN_TRUE);
                }
            }
            break;
        case 'w': return keyword_check(1, 4, "hile", TOKEN_WHILE);
    }

    return TOKEN_IDENTIFIER;
}

static Token identifier_make()
{
    while (is_alpha(peek()) || is_digit(peek())) advance();

    return token_make(identifier_type());
}

/**
 * Make a token for a digit
 *
 * We consume the digit here, including 
 * cases where we have a double. Later, 
 * the compiler will conver the lexeme into 
 * the number.
 */
static Token number_make()
{
    while (is_digit(peek())) advance();

    // Look for a fractional part
    if (peek() == '.' && is_digit(peek_next()))
    {
        // consume the "."
        advance();
    }

    while (is_digit(peek())) advance();

    return token_make(TOKEN_NUMBER);
}

static Token string_make()
{
    while (peek() != '"' && !is_at_end())
    {
        if (peek() == '\n') scanner.line++;
        advance();
    }

    if (is_at_end()) return token_error("Unterminated string.");

    // The closing quote
    advance();
    return token_make(TOKEN_STRING);
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
    if (is_alpha(c)) return identifier_make();
    if (is_digit(c)) return number_make();

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
        // Literals
        case '"': return string_make();
    }

    return token_error("Unexpected character.");
}
