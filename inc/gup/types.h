/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_TYPES_H
#define GUP_TYPES_H 1

typedef enum {
    GUP_TYPE_BAD,
    GUP_TYPE_VOID,
    GUP_TYPE_U8,
    GUP_TYPE_U16,
    GUP_TYPE_U32,
    GUP_TYPE_U64,
    GUP_TYPE_MAX
} gup_type_t;

#endif  /* !GUP_TYPES_H */
