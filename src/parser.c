/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include "gup/parser.h"
#include "gup/lexer.h"
#include "gup/parser.h"
#include "gup/ptrbox.h"
#include "gup/trace.h"

/*
 * Table used to convert token constants to string
 * literals.
 */
static const char *toktab[] = {
    [TT_NONE]       = "NONE",
    [TT_PLUS]       = "PLUS",
    [TT_MINUS]      = "MINUS",
    [TT_SLASH]      = "SLASH",
    [TT_STAR]       = "STAR",
    [TT_EQUALS]     = "EQUALS",
    [TT_EQUALITY]   = "EQUALITY",
    [TT_LT]         = "LESS-THAN",
    [TT_LTE]        = "LESS-THAN-OR-EQUALS",
    [TT_GT]         = "GREATER-THAN",
    [TT_GTE]        = "GREATER-THAN-OR-EQUALS",
    [TT_NUMBER]     = "NUMBER",
    [TT_IDENT]      = "IDENTIFIER",
    [TT_FN]         = "FUNCTION",
    [TT_SEMI]       = "SEMICOLON"
};

/*
 * Assert that the next token is of a specific kind
 *
 * @state: Compiler state
 * @tok:   Token result
 * @what:  Token type to assert
 */
static int
parse_expect(struct gup_state *state, struct token *tok, tt_t what)
{
    if (state == NULL || tok == NULL) {
        errno = -EINVAL;
        return -1;
    }

    if (lexer_scan(state, tok) < 0) {
        trace_error(
            state,
            "got unexpected end of file, expected %s\n",
            toktab[what]
        );
        return -1;
    }

    if (tok->type != what) {
        trace_error(
            state,
            "expected %s, got %s instead\n",
            toktab[what],
            toktab[tok->type]
        );

        return -1;
    }

    return 0;
}

static int
begin_parse(struct gup_state *state, struct token *tok)
{
    if (state == NULL || tok == NULL) {
        errno = -EINVAL;
        return -1;
    }

    switch (tok->type) {
    case TT_FN:
        if (parse_expect(state, tok, TT_IDENT) < 0) {
            return -1;
        }

        if (parse_expect(state, tok, TT_MINUS) < 0) {
            return -1;
        }

        if (parse_expect(state, tok, TT_GT) < 0) {
            return -1;
        }

        /* TODO: This should be a type */
        if (parse_expect(state, tok, TT_IDENT) < 0) {
            return -1;
        }

        if (parse_expect(state, tok, TT_SEMI) < 0) {
            return -1;
        }

        break;
    default:
        return 0;
    }

    return 0;
}

int
gup_parse(struct gup_state *state)
{
    struct token token;
    int error;

    if (state == NULL) {
        errno = -EINVAL;
        return -1;
    }

    if ((error = ptrbox_init(&state->ptrbox)) < 0) {
        return -1;
    }

    if ((error = ptrbox_init(&state->ast_ptrbox)) < 0) {
        ptrbox_destroy(&state->ptrbox);
        return -1;
    }

    if (symbol_table_init(&state->g_symtab) < 0) {
        ptrbox_destroy(&state->ast_ptrbox);
        ptrbox_destroy(&state->ptrbox);
        return -1;
    }

    while (lexer_scan(state, &token) == 0) {
        trace_debug("got token: %s\n", toktab[token.type]);
        if (begin_parse(state, &token) < 0) {
            break;
        }
    }

    symbol_table_destroy(&state->g_symtab);
    ptrbox_destroy(&state->ast_ptrbox);
    ptrbox_destroy(&state->ptrbox);
    return 0;
}
