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
    [TT_FN]         = "FUNCTION"
};

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
        return -1;
    }

    while (lexer_scan(state, &token) == 0) {
        printf("got token: %s\n", toktab[token.type]);
    }

    ptrbox_destroy(&state->ast_ptrbox);
    ptrbox_destroy(&state->ptrbox);
    return 0;
}
