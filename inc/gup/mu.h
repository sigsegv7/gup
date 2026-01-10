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
    MACH_REGSIZE_BAD,
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

/*
 * Return a void value
 *
 * @state: Compiler state
 */
int mu_cg_retvoid(struct gup_state *state);

/*
 * Emit a call to a function symbol
 *
 * @state: Compiler state
 * @label: Label to call
 */
int mu_cg_call(struct gup_state *state, const char *label);

/*
 * Emit a jump to a label
 *
 * @state: Compiler state
 * @label: Label to jump to
 */
int mu_cg_jmp(struct gup_state *state, const char *label);

/*
 * Emit a struct
 *
 * @state: Compiler state
 * @name: Name suffix
 * @node: Root of struct
 */
int mu_cg_struct(struct gup_state *state, const char *name, struct ast_node *node);

/*
 * Emit a loop start
 *
 * @state: Compiler state
 */
int mu_cg_loopstart(struct gup_state *state);

/*
 * Emit a label
 *
 * @state: Compiler state
 * @name: Label name
 */
int mu_cg_label(struct gup_state *state, const char *name);

#endif  /* !GUP_MU_H */
