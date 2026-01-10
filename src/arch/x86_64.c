/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include "gup/mu.h"

/*
 * Return value registers according to the system V
 * ABI
 */
static const char *retregs[] = {
    [MACH_REGSIZE_64] = "rax",
    [MACH_REGSIZE_32] = "eax",
    [MACH_REGSIZE_16] = "ah",
    [MACH_REGSIZE_8]  = "al"
};

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

int
mu_cg_retimm(struct gup_state *state, regsize_t regsize, ssize_t imm)
{
    if (state == NULL || regsize >= MACH_REGSIZE_MAX) {
        errno = -EINVAL;
        return -1;
    }

    fprintf(
        state->out_fp,
        "\tmov %s, %zd\n"
        "\tret\n",
        retregs[regsize],
        imm
    );

    return 0;
}

int
mu_cg_retvoid(struct gup_state *state)
{
    if (state == NULL) {
        errno = -EINVAL;
        return -1;
    }

    fprintf(
        state->out_fp,
        "\tret\n"
    );

    return 0;
}
