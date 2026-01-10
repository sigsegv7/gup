/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_AST_H
#define GUP_AST_H 1

#include <stdint.h>
#include <stddef.h>
#include "gup/state.h"
#include "gup/symbol.h"

/*
 * Represents valid AST node types
 *
 * @AST_OP_NONE: No type
 * @AST_OP_FUNC: Is a function
 * @AST_OP_ASM: Inline assembly
 */
typedef enum {
    AST_OP_NONE,
    AST_OP_FUNC,
    AST_OP_ASM
} ast_op_t;

/*
 * Represents a single node within an abstract syntax
 * tree.
 *
 * @type: AST node type
 * @left: Left leaf
 * @right: Right leaf
 * @symbol: Symbol this node refers to
 */
struct ast_node {
    ast_op_t type;
    struct ast_node *left;
    struct ast_node *right;
    struct symbol *symbol;
    union {
        char *str;
    };
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
