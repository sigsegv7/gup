/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <errno.h>
#include <stddef.h>
#include "gup/trace.h"
#include "gup/codegen.h"
#include "gup/symbol.h"
#include "gup/mu.h"

int
cg_compile_node(struct gup_state *state, struct ast_node *node)
{
    struct symbol *symbol;

    if (node == NULL) {
        errno = -EINVAL;
        return -1;
    }

    switch (node->type) {
    case AST_OP_FUNC:
        if ((symbol = node->symbol) == NULL) {
            errno = -EIO;
            trace_error(state, "[AST] function has no symbol\n");
            return -1;
        }

        trace_debug("[AST] detected function %s\n", symbol->name);
        mu_cg_funcp(state, symbol->name, symbol->is_pub);
        break;
    case AST_OP_ASM:
        if (node->str == NULL) {
            return -1;
        }
        mu_cg_asm(state, node->str);
        break;
    case AST_OP_RETVOID:
        mu_cg_retvoid(state);
        break;
    default:
        trace_error(state, "[AST]: bad node type %d\n", node->type);
        return -1;
    }

    return -1;
}
