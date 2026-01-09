/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_CODEGEN_H
#define GUP_CODEGEN_H 1

#include "gup/ast.h"
#include "gup/state.h"

/*
 * Traverse an AST node based on its type and generate
 * machine sources.
 *
 * @state: Compiler state
 * @node: Node to compile
 */
int cg_compile_node(struct gup_state *state, struct ast_node *node);

#endif  /* !GUP_CODEGEN_H */
