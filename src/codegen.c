/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <errno.h>
#include <stddef.h>
#include "gup/types.h"
#include "gup/trace.h"
#include "gup/codegen.h"
#include "gup/symbol.h"
#include "gup/mu.h"

static inline regsize_t
dtype_to_regsize(gup_type_t type)
{
    switch (type) {
    case GUP_TYPE_U8:
        return MACH_REGSIZE_8;
    case GUP_TYPE_U16:
        return MACH_REGSIZE_16;
    case GUP_TYPE_U32:
        return MACH_REGSIZE_32;
    case GUP_TYPE_U64:
        return MACH_REGSIZE_64;
    default:
        return MACH_REGSIZE_BAD;
    }
}

static int
cg_compile_struct(struct gup_state *state, const char *name, struct ast_node *node)
{
    if (state == NULL || node == NULL) {
        errno = -EINVAL;
        return -1;
    }

    mu_cg_struct(state, name, node);
    return 0;
}

int
cg_compile_node(struct gup_state *state, struct ast_node *node)
{
    struct symbol *symbol;
    char label[32];

    if (node == NULL) {
        errno = -EINVAL;
        return -1;
    }

    switch (node->type) {
    case AST_OP_FUNC:
        if ((symbol = node->symbol) == NULL) {
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
    case AST_OP_RETIMM:
        if ((symbol = state->this_func) == NULL) {
            return -1;
        }
        mu_cg_retimm(state, dtype_to_regsize(symbol->data_type), node->v);
        break;
    case AST_OP_CALL:
        if ((symbol = node->symbol) == NULL) {
            return -1;
        }

        if (symbol->type != SYMBOL_TYPE_FUNC) {
            trace_error(state, "[AST] %s is not a function\n", symbol->name);
            return -1;
        }

        mu_cg_call(state, symbol->name);
        break;
    case AST_OP_STRUCT:
        cg_compile_struct(state, node->str, node);
        break;
    case AST_OP_LOOP:
        if (!node->epilogue) {
            mu_cg_loopstart(state);
        } else {
            snprintf(label, sizeof(label), "L.%zu", state->loop_count - 1);
            mu_cg_jmp(state, label);
        }
        break;
    default:
        trace_error(state, "[AST]: bad node type %d\n", node->type);
        return -1;
    }

    return -1;
}
