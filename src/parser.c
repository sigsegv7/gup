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
    [TT_NUMBER]     = "NUMBER"
};

int
gup_parse(struct gup_state *state)
{
    struct token token;

    if (state == NULL) {
        errno = -EINVAL;
        return -1;
    }

    while (lexer_scan(state, &token) == 0) {
        printf("got token: %s\n", toktab[token.type]);
    }

    return 0;
}
