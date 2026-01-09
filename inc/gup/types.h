/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_TYPES_H
#define GUP_TYPES_H 1

typedef uint8_t gup_type_t;

#define GUP_TYPE_BAD    0x00
#define GUP_TYPE_VOID   0x01
#define GUP_TYPE_U8     0x02
#define GUP_TYPE_U16    0x03
#define GUP_TYPE_U32    0x04
#define GUP_TYPE_U64    0x05

#endif  /* !GUP_TYPES_H */
