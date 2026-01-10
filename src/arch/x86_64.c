/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include "gup/mu.h"

int
mu_cg_funcp(struct gup_state *state, const char *name, bool is_global)
{
    if (state == NULL || name == NULL) {
        errno = -EINVAL;
        return -1;
    }

    if (is_global) {
        fprintf(state->out_fp, "[global %s]\n", name);
    }

    fprintf(
        state->out_fp,
        "%s:\n",
        name
    );
    return 0;
}

int
mu_cg_asm(struct gup_state *state, const char *asm_str)
{
    if (state == NULL || asm_str == NULL) {
        errno = -EINVAL;
        return -1;
    }

    fprintf(
        state->out_fp,
        "\t%s\n",
        asm_str
    );

    return 0;
}
