/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_STATE_H
#define GUP_STATE_H 1

#include <stdint.h>
#include <stdio.h>
#include "gup/ptrbox.h"
#include "gup/symbol.h"

#define ASMOUT_DEFAULT "gupgen.asm"

/*
 * Represents the compiler state
 *
 * @in_fd: Input file pointer
 * @putback: Putback buffer ('\0' if empty)
 * @line_num: Line number
 * @ptrbox: Parser pointer box
 * @ast_ptrbox: Pointer box for AST
 * @g_symtab: Global symbol table
 * @out_fp: Output file pointer
 */
struct gup_state {
    int in_fd;
    char putback;
    size_t line_num;
    struct ptrbox ptrbox;
    struct ptrbox ast_ptrbox;
    struct symbol_table g_symtab;
    FILE *out_fp;
};

/*
 * Open a new GUP state
 *
 * @path: Path of input file
 * @res: Resulting state descriptor written here
 *
 * Returns zero on success
 */
int gup_open(const char *path, struct gup_state *res);

/*
 * Close a GUP state
 *
 * @state: GUP state to close
 */
void gup_close(struct gup_state *state);

#endif  /* !GUP_STATE_H */
