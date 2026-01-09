#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include "gup/lexer.h"

/*
 * Returns true if the input character 'c' is a whitespace
 * character
 *
 * @c: Input character to check
 */
static inline bool
is_whitespace(char c)
{
    switch (c) {
    case ' ':
    case '\n':
    case '\r':
    case '\t':
    case '\f':
        return true;
    default:
        return false;
    }
}

/*
 * Consume a single character from the input file
 *
 * @state: Compiler state
 * @allow_ws: If true, allow whitespace
 */
static char
lexer_nom(struct gup_state *state, bool allow_ws)
{
    char c = '\0';

    if (state == NULL) {
        return '\0';
    }

    /*
     * If there is data in the putback buffer, pull it out
     * and return it if it is not a whitespace character.
     */
    if (state->putback != '\0') {
        c = state->putback;
        state->putback = '\0';
        if (!is_whitespace(c))
            return c;
    }

    while (read(state->in_fd, &c, 1) > 0) {
        if (c == '\n') {
            ++state->line_num;
            continue;
        }

        /*
         * If we encountered a whitespace character and are
         * accepting them, take it.
         */
        if (is_whitespace(c) && allow_ws) {
            break;
        }

        /*
         * If we are not accepting whitespace characters and encounter
         * a non-whitespace character, take it.
         */
        if (!is_whitespace(c)) {
            break;
        }
    }

    return c;
}

int
lexer_scan(struct gup_state *state, struct token *res)
{
    char c;

    if (state == NULL || res == NULL) {
        errno = -EINVAL;
        return -1;
    }

    if ((c = lexer_nom(state, false)) == '\0') {
        return -1;
    }

    switch (c) {
    case '+':
        res->type = TT_PLUS;
        res->c = c;
        return 0;
    case '-':
        res->type = TT_MINUS;
        res->c = c;
        return 0;
    case '/':
        res->type = TT_SLASH;
        res->c = c;
        return 0;
    case '*':
        res->type = TT_STAR;
        res->c = c;
        return 0;
    case '=':
        res->type = TT_EQUALS;
        res->c = c;
        if ((c = lexer_nom(state, false)) != '=') {
            state->putback = c;
            return 0;
        }
        res->type = TT_EQUALITY;
        return 0;
    case '<':
        res->type = TT_LT;
        res->c = c;
        return 0;
    case '>':
        res->type = TT_GT;
        res->c = c;
        return 0;
    }

    return -1;
}
