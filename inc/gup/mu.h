/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_MU_H
#define GUP_MU_H 1

#include <sys/types.h>
#include <stdbool.h>
#include "gup/state.h"
/*
 * Valid machine register sizes
 */
typedef enum {
    MACH_REGSIZE_8,
    MACH_REGSIZE_16,
    MACH_REGSIZE_32,
    MACH_REGSIZE_64,
    MACH_REGSIZE_MAX
} regsize_t;

/*
 * Generate a function prolouge
 *
 * @state: Compiler state
 * @name: Name of function to generate
 * @is_global: If true, function is global
 */
int mu_cg_funcp(struct gup_state *state, const char *name, bool is_global);

/*
 * Inject inline assembly
 *
 * @state: Compiler state
 * @asm_str: Assembly to inject
 */
int mu_cg_asm(struct gup_state *state, const char *asm_str);

/*
 * Load a return value register with respect to the platform
 * ABI.
 *
 * @state: Compiler state
 * @regsize: Register size
 * @imm: Immediate value to return
 */
int mu_cg_retimm(struct gup_state *state, regsize_t regsize, ssize_t imm);

#endif  /* !GUP_MU_H */
