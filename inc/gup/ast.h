/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_AST_H
#define GUP_AST_H 1

#include <stdint.h>
#include <stddef.h>
#include "gup/state.h"

/*
 * Represents valid AST node types
 *
 * @AST_OP_NONE: No type
 */
typedef enum {
    AST_OP_NONE,
} ast_op_t;

/*
 * Represents a single node within an abstract syntax
 * tree.
 *
 * @type: AST node type
 * @left: Left leaf
 * @right: Right leaf
 */
struct ast_node {
    ast_op_t type;
    struct ast_node *left;
    struct ast_node *right;
};

/*
 * Allocate a new AST node
 *
 * @state: Compiler state
 * @type: AST type
 * @res: Result node is written here
 *
 * Returns zero on success
 */
int ast_node_alloc(struct gup_state *state, ast_op_t type, struct ast_node **res);

#endif  /* !GUP_AST_H */
