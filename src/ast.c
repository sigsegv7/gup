#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include "gup/ast.h"
#include "gup/ptrbox.h"

int
ast_node_alloc(struct gup_state *state, ast_op_t type, struct ast_node **res)
{
    struct ast_node *node;

    if (state == NULL || res == NULL) {
        errno = -EINVAL;
        return -1;
    }

    node = ptrbox_alloc(&state->ast_ptrbox, sizeof(*node));
    if (node == NULL) {
        errno = -ENOMEM;
        return -1;
    }

    node->type = type;
    node->left = NULL;
    node->right = NULL;
    node->symbol = NULL;
    *res = node;
    return 0;
}
