/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include "gup/codegen.h"
#include "gup/parser.h"
#include "gup/lexer.h"
#include "gup/parser.h"
#include "gup/ptrbox.h"
#include "gup/trace.h"
#include "gup/types.h"
#include "gup/ast.h"

/*
 * Table used to convert token constants to string
 * literals.
 */
static const char *toktab[] = {
    [TT_NONE]       = "NONE",
    [TT_PLUS]       = "PLUS",
    [TT_MINUS]      = "MINUS",
    [TT_SLASH]      = "SLASH",
    [TT_STAR]       = "STAR",
    [TT_EQUALS]     = "EQUALS",
    [TT_EQUALITY]   = "EQUALITY",
    [TT_LT]         = "LESS-THAN",
    [TT_LTE]        = "LESS-THAN-OR-EQUALS",
    [TT_GT]         = "GREATER-THAN",
    [TT_GTE]        = "GREATER-THAN-OR-EQUALS",
    [TT_LPAREN]     = "LPAREN",
    [TT_RPAREN]     = "RPAREN",
    [TT_LBRACE]     = "LBRACE",
    [TT_RBRACE]     = "RBRACE",
    [TT_NUMBER]     = "NUMBER",
    [TT_IDENT]      = "IDENTIFIER",
    [TT_FN]         = "FUNCTION",
    [TT_PUB]        = "PUB",
    [TT_SEMI]       = "SEMICOLON",
    [TT_VOID]       = "VOID",
    [TT_U8]         = "U8",
    [TT_U16]        = "U16",
    [TT_U32]        = "U32",
    [TT_U64]        = "U64",
    [TT_STRING]     = "STRING",
    [TT_ASM]        = "ASM"
};

/*
 * Convert a type specific token into a type constant
 *
 * @tok: Token to convert
 */
static inline gup_type_t
token_to_type(tt_t tok)
{
    switch (tok) {
    case TT_VOID:
        return GUP_TYPE_VOID;
    case TT_U8:
        return GUP_TYPE_U8;
    case TT_U16:
        return GUP_TYPE_U16;
    case TT_U32:
        return GUP_TYPE_U32;
    case TT_U64:
        return GUP_TYPE_U64;
    default:
        return GUP_TYPE_BAD;
    }
}

/*
 * Assert that the next token is of a specific kind
 *
 * @state: Compiler state
 * @tok:   Token result
 * @what:  Token type to assert
 */
static int
parse_expect(struct gup_state *state, struct token *tok, tt_t what)
{
    if (state == NULL || tok == NULL) {
        errno = -EINVAL;
        return -1;
    }

    if (lexer_scan(state, tok) < 0) {
        trace_error(
            state,
            "got unexpected end of file, expected %s\n",
            toktab[what]
        );
        return -1;
    }

    if (tok->type != what) {
        trace_error(
            state,
            "expected %s, got %s instead\n",
            toktab[what],
            toktab[tok->type]
        );

        return -1;
    }

    return 0;
}

static int
parse_function(struct gup_state *state, struct token *tok)
{
    struct ast_node *root;
    struct token *last_tok;
    struct symbol *symbol;
    symid_t sym_id;
    gup_type_t type;

    if (parse_expect(state, tok, TT_IDENT) < 0) {
        return -1;
    }

    sym_id = symbol_new(
        &state->g_symtab,
        tok->s,
        SYMBOL_TYPE_FUNC,
        &symbol
    );

    if (sym_id < 0) {
        trace_error(state, "failed to create symbol for \"%s\"\n", tok->s);
        return -1;
    }

    /*
     * If the token before the function was 'pub' then we shall
     * mark this function as public.
     */
    last_tok = &state->last_token;
    if (last_tok->type == TT_PUB) {
        symbol->is_pub = 1;
    }

    if (parse_expect(state, tok, TT_MINUS) < 0) {
        return -1;
    }

    if (parse_expect(state, tok, TT_GT) < 0) {
        return -1;
    }

    if (lexer_scan(state, tok) < 0) {
        trace_error(state, "expected TYPE after '->'\n");
        return -1;
    }

    type = token_to_type(tok->type);
    if (type == GUP_TYPE_BAD) {
        trace_error(
            state,
            "expected TYPE after '->', got %s\n",
            toktab[tok->type]
        );
        return -1;
    }

    if (lexer_scan(state, tok) < 0) {
        return -1;
    }

    /*
     * If there is a semicolon instead of an open LBRACE, it is simply
     * and declaration
     */
    switch (tok->type) {
    case TT_SEMI:
        return 0;
    case TT_LBRACE:
        state->this_func = symbol;
        break;
    default:
        trace_error(state, "expected LBRACE or SEMICOLON after func\n");
        return -1;
    }

    symbol->data_type = type;
    if (ast_node_alloc(state, AST_OP_FUNC, &root) < 0) {
        trace_error(state, "failed to allocate ast node for function\n");
        return -1;
    }

    root->symbol = symbol;
    cg_compile_node(state, root);
    return 0;
}

static int
parse_asm(struct gup_state *state, struct token *tok)
{
    struct ast_node *root;

    if (parse_expect(state, tok, TT_LPAREN) < 0) {
        return -1;
    }

    if (parse_expect(state, tok, TT_STRING) < 0) {
        return -1;
    }

    /* Allocate an ASM block AST node */
    if (ast_node_alloc(state, AST_OP_ASM, &root) < 0) {
        trace_error(state, "failed to allocate ast node for function\n");
        return -1;
    }

    /* Copy the contents and compile the node */
    root->str = ptrbox_strdup(&state->ptrbox, tok->s);
    cg_compile_node(state, root);

    if (parse_expect(state, tok, TT_RPAREN) < 0) {
        return -1;
    }

    if (parse_expect(state, tok, TT_SEMI) < 0) {
        return -1;
    }

    return 0;
}

static int
begin_parse(struct gup_state *state, struct token *tok)
{
    if (state == NULL || tok == NULL) {
        errno = -EINVAL;
        return -1;
    }

    switch (tok->type) {
    case TT_FN:
        if (parse_function(state, tok) < 0) {
            return -1;
        }
        break;
    case TT_ASM:
        if (parse_asm(state, tok) < 0) {
            return -1;
        }
        break;
    case TT_PUB:
        break;
    case TT_RBRACE:
        if (state->this_func == NULL) {
            trace_error(state, "unexpected RBRACE\n");
            return -1;
        }

        state->this_func = NULL;
        break;
    default:
        break;
    }

    state->last_token = *tok;
    return 0;
}

int
gup_parse(struct gup_state *state)
{
    struct token token;
    int error = 0;

    if (state == NULL) {
        errno = -EINVAL;
        return -1;
    }

    if ((error = ptrbox_init(&state->ptrbox)) < 0) {
        return -1;
    }

    if ((error = ptrbox_init(&state->ast_ptrbox)) < 0) {
        ptrbox_destroy(&state->ptrbox);
        return -1;
    }

    if (symbol_table_init(&state->g_symtab) < 0) {
        ptrbox_destroy(&state->ast_ptrbox);
        ptrbox_destroy(&state->ptrbox);
        return -1;
    }

    while (lexer_scan(state, &token) == 0) {
        trace_debug("got token: %s\n", toktab[token.type]);
        if (begin_parse(state, &token) < 0) {
            break;
        }
    }

    if (state->this_func != NULL) {
        trace_error(state, "unexpected end of file, missing RBRACE?\n");
        error = -1;
    }

    symbol_table_destroy(&state->g_symtab);
    ptrbox_destroy(&state->ast_ptrbox);
    ptrbox_destroy(&state->ptrbox);
    return error;
}
