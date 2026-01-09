/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_MU_H
#define GUP_MU_H 1

#include <stdbool.h>
#include "gup/state.h"

/*
 * Generate a function prolouge
 *
 * @state: Compiler state
 * @name: Name of function to generate
 * @is_global: If true, function is global
 */
int mu_cg_funcp(struct gup_state *state, const char *name, bool is_global);

#endif  /* !GUP_MU_H */
