/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_TOKEN_H
#define GUP_TOKEN_H 1

#include <stdint.h>
#include <stddef.h>

/*
 * Represents valid token types, these are constants
 * that are quick to compare.
 */
typedef enum {
    TT_NONE,        /* <NONE> */
    TT_PLUS,        /* '+' */
    TT_MINUS,       /* '-' */
    TT_SLASH,       /* '/' */
    TT_STAR,        /* '*' */
    TT_EQUALS,      /* '=' */
    TT_EQUALITY,    /* '==' */
    TT_LT,          /* '<' */
    TT_LTE,         /* '<=' */
    TT_GT,          /* '>' */
    TT_GTE,         /* '>=' */
    TT_NUMBER,      /* <NUMBER> */
    TT_IDENT,       /* <IDENT> */
    TT_FN,          /* 'fn' */
    TT_SEMI,        /* ';' */
    TT_VOID,        /* 'void' */
    TT_U8,          /* 'u8' */
    TT_U16,         /* 'u16' */
    TT_U32,         /* 'u32' */
    TT_U64,         /* 'u64' */
} tt_t;

/*
 * Represents a single lexical token
 *
 * @type: Token type
 */
struct token {
    tt_t type;
    union {
        char c;
        size_t v;
        char *s;
    };
};

#endif  /* !GUP_TOKEN_H */
