#include <sys/queue.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "gup/symbol.h"

int
symbol_table_init(struct symbol_table *table)
{
    if (table == NULL) {
        errno = -EINVAL;
        return -1;
    }

    TAILQ_INIT(&table->symbols);
    table->symbol_count = 0;
    return 0;
}

symid_t symbol_new(struct symbol_table *tbl, const char *name, symtype_t type,
    struct symbol **res)
{
    struct symbol *symbol;

    if (tbl == NULL || name == NULL) {
        errno = -EINVAL;
        return -1;
    }

    if ((symbol = malloc(sizeof(*symbol))) == NULL) {
        errno = -ENOMEM;
        return -1;
    }

    if ((symbol->name = strdup(name)) == NULL) {
        free(symbol);
        errno = -ENOMEM;
        return -1;
    }

    symbol->type = type;
    symbol->data_type = GUP_TYPE_VOID;
    symbol->id = tbl->symbol_count++;
    symbol->is_pub = 0;
    TAILQ_INSERT_TAIL(&tbl->symbols, symbol, link);
    if (res != NULL) {
        *res = symbol;
    }
    return symbol->id;
}

void
symbol_table_destroy(struct symbol_table *table)
{
    struct symbol *symbol;

    if (table == NULL) {
        return;
    }

    if ((symbol = TAILQ_FIRST(&table->symbols)) == NULL) {
        return;
    }

    while (symbol != NULL) {
        TAILQ_REMOVE(&table->symbols, symbol, link);
        if (symbol->name != NULL) {
            free(symbol->name);
        }
        free(symbol);
        symbol = TAILQ_FIRST(&table->symbols);
    }
}

struct symbol *
symbol_from_id(struct symbol_table *tbl, symid_t id)
{
    struct symbol *symbol;

    if (tbl == NULL) {
        return NULL;
    }

    TAILQ_FOREACH(symbol, &tbl->symbols, link) {
        if (symbol->id == id) {
            return symbol;
        }
    }

    return NULL;
}
