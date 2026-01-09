/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_SYMBOL_H
#define GUP_SYMBOL_H 1

#include <sys/queue.h>
#include <stdint.h>
#include <stddef.h>
#include "gup/symbol.h"
#include "gup/types.h"

typedef int32_t symid_t;

/*
 * Returns valid symbol types
 *
 * @SYMBOL_TYPE_NONE: No type
 * @SYMBOL_TYPE_FUNC: Is a function
 */
typedef enum {
    SYMBOL_TYPE_NONE,
    SYMBOL_TYPE_FUNC
} symtype_t;

/*
 * Represents a program symbol
 *
 * @name: Symbol name (strdup'd)
 * @type: Symbol type
 * @data_type: Data type of symbol
 * @link: Queue link
 */
struct symbol {
    char *name;
    symtype_t type;
    gup_type_t data_type;
    symid_t id;
    TAILQ_ENTRY(symbol) link;
};

/*
 * Represents a symbol table that references one or
 * more program symbols
 *
 * @symbols: List of program symbols
 * @symbol_count: Number or program symbols
 */
struct symbol_table {
    TAILQ_HEAD(, symbol) symbols;
    size_t symbol_count;
};

/*
 * Allocate a new symbol and add it to a symbol
 * table
 *
 * @tbl: Symbol table to add to
 * @name: Name of symbol to create
 * @type: Type of symbol to create
 * @res: Result is written here
 */
symid_t symbol_new(
    struct symbol_table *tbl, const char *name,
    symtype_t type, struct symbol **res
);

/*
 * Obtain a symbol from its ID
 *
 * @tbl: Symbol table to look up within
 * @id: ID to look up
 *
 * Returns NULL if not found
 */
struct symbol *symbol_from_id(struct symbol_table *tbl, symid_t id);

/*
 * Initialize a symbol table
 *
 * @table: Symbol table to initialize
 *
 * Returns zero on success
 */
int symbol_table_init(struct symbol_table *table);

/*
 * Destroy a symbol table
 *
 * @table: Symbol table to destroy
 */
void symbol_table_destroy(struct symbol_table *table);

#endif  /* !GUP_SYMBOL_H */
