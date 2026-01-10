/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <stdint.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include "gup/lexer.h"
#include "gup/trace.h"

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
    ssize_t len;

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

    while ((len = read(state->in_fd, &c, 1)) > 0) {
        if (c == '\n') {
            ++state->line_num;
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

    if (len == 0) {
        c = '\0';
    }

    return c;
}

static int
lexer_scan_ident(struct gup_state *state, int lc, struct token *res)
{
    char buf[MAX_IDENT_LEN];
    size_t buf_i = 0;
    int c;

    if (!isalpha(lc) && lc != '_') {
        return -1;
    }

    buf[buf_i++] = lc;
    for (;;) {
        c = lexer_nom(state, true);
        if (!isalnum(c) && c != '_') {
            state->putback = c;
            buf[buf_i] = '\0';
            break;
        }

        buf[buf_i++] = c;
        if (buf_i >= sizeof(buf) - 1) {
            buf[buf_i] = '\0';
            break;
        }
    }

    res->type = TT_IDENT;
    res->s = ptrbox_strdup(&state->ptrbox, buf);
    return 0;
}

static int
lexer_scan_digits(struct gup_state *state, int lc, struct token *res)
{
    char buf[21];
    uint8_t buf_i = 0;
    int c;

    if (state == NULL || res == NULL) {
        errno = -EINVAL;
        return -1;
    }

    if (!isdigit(lc)) {
        return -1;
    }

    buf[buf_i++] = lc;
    for (;;) {
        /*
         * Consume a single character from the input, if it
         * is not a digit, put it back.
         */
        c = lexer_nom(state, false);
        if (!isdigit(c)) {
            state->putback = c;
            buf[buf_i] = '\0';
            break;
        }

        buf[buf_i++] = c;
        if (buf_i >= sizeof(buf) - 1) {
            buf[buf_i] = '\0';
            break;
        }
    }

    res->type = TT_NUMBER;
    res->v = atoi(buf);
    return 0;
}

/*
 * Scan for a string in the source input
 *
 * @state: Compiler state
 * @res: Token result
 *
 * Returns zero on success
 */
static int
lexer_scan_str(struct gup_state *state, struct token *res)
{
    char *buf;
    size_t buf_size, buf_cap;
    int c;

    if (state == NULL || res == NULL) {
        errno = -EINVAL;
        return -1;
    }

    buf_size = 0;
    buf_cap = 8;
    if ((buf = malloc(buf_cap)) == NULL) {
        errno = -ENOMEM;
        return -1;
    }

    for (;;) {
        c = lexer_nom(state, true);
        if (c == '\0') {
            trace_error(state, "got unexpected end of file\n");
            trace_warn("unterminated string?\n");
            free(buf);
            return -1;
        }

        if (c == '"') {
            buf[buf_size] = '\0';
            break;
        }

        buf[buf_size++] = c;
        if (buf_size >= buf_cap - 1) {
            buf_cap += 8;
            buf = realloc(buf, buf_cap);
        }

        if (buf == NULL) {
            trace_error(state, "out of memory\n");
            return -1;
        }
    }

    res->type = TT_STRING;
    res->s = ptrbox_strdup(&state->ptrbox, buf);
    free(buf);
    return 0;
}

/*
 * Check if a token is actually a keyword rather than an
 * identifier
 *
 * @state: Compiler state
 * @tok: Token to check
 */
static int
lexer_check_kw(struct gup_state *state, struct token *tok)
{
    if (state == NULL || tok == NULL) {
        errno = -EINVAL;
        return -1;
    }

    /*
     * We only want to see if an identifier is actually a keyword,
     * this will not work with any other kind of token.
     */
    if (tok->type != TT_IDENT) {
        return -1;
    }

    switch (*tok->s) {
    case '_':
        if (strcmp(tok->s, "__asm") == 0) {
            tok->type = TT_ASM;
            return 0;
        }

        break;
    case 'f':
        if (strcmp(tok->s, "fn") == 0) {
            tok->type = TT_FN;
            return 0;
        }

        break;
    case 'v':
        if (strcmp(tok->s, "void") == 0) {
            tok->type = TT_VOID;
            return 0;
        }

        break;
    case 'u':
        if (strcmp(tok->s, "u8") == 0) {
            tok->type = TT_U8;
            return 0;
        }

        if (strcmp(tok->s, "u16") == 0) {
            tok->type = TT_U16;
            return 0;
        }

        if (strcmp(tok->s, "u32") == 0) {
            tok->type = TT_U32;
            return 0;
        }

        if (strcmp(tok->s, "u64") == 0) {
            tok->type = TT_U64;
            return 0;
        }

        break;
    case 'p':
        if (strcmp(tok->s, "pub") == 0) {
            tok->type = TT_PUB;
            return 0;
        }

        break;
    case 'r':
        if (strcmp(tok->s, "return") == 0) {
            tok->type = TT_RETURN;
            return 0;
        }

        break;
    case 's':
        if (strcmp(tok->s, "struct") == 0) {
            tok->type = TT_STRUCT;
            return 0;
        }

        break;
    case 'l':
        if (strcmp(tok->s, "loop") == 0) {
            tok->type = TT_LOOP;
            return 0;
        }

        break;
    case 'b':
        if (strcmp(tok->s, "break") == 0) {
            tok->type = TT_BREAK;
            return 0;
        }

        break;
    }

    return -1;
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
        if ((c = lexer_nom(state, false)) != '=') {
            state->putback = c;
            return 0;
        }

        res->type = TT_LTE;
        return 0;
    case '>':
        res->type = TT_GT;
        res->c = c;
        if ((c = lexer_nom(state, false)) != '=') {
            state->putback = c;
            return 0;
        }

        res->type = TT_GTE;
        return 0;
    case '(':
        res->type = TT_LPAREN;
        res->c = c;
        return 0;
    case ')':
        res->type = TT_RPAREN;
        res->c = c;
        return 0;
    case ';':
        res->type = TT_SEMI;
        res->c = c;
        return 0;
    case '{':
        res->type = TT_LBRACE;
        res->c = c;
        return 0;
    case '}':
        res->type = TT_RBRACE;
        res->c = c;
        return 0;
    default:
        /*
         * If we simply have a quote, it is assumed to be a
         * string.
         */
        if (c == '"') {
            if (lexer_scan_str(state, res) == 0)
                return 0;
        }

        /* Are these digits? */
        if (lexer_scan_digits(state, c, res) == 0) {
            return 0;
        }

        /* An identifier? */
        if (lexer_scan_ident(state, c, res) == 0) {
            lexer_check_kw(state, res);
            return 0;
        }

        trace_error(state, "bad token '%c'\n", c);
        return -1;
    }

    return -1;
}
