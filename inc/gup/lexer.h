/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_LEXER_H
#define GUP_LEXER_H 1

#include <stdint.h>
#include <stddef.h>
#include "gup/state.h"
#include "gup/token.h"

#define MAX_IDENT_LEN 256

/*
 * Scan a single token and discard any whitespace
 *
 * @state: Compiler state
 * @res: Token result written here
 *
 * Returns zero on success
 */
int lexer_scan(struct gup_state *state, struct token *res);

#endif  /* !GUP_LEXER_H */
