/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#ifndef GUP_STATE_H
#define GUP_STATE_H 1

#include <stdint.h>
#include <stdio.h>

/*
 * Represents the compiler state
 *
 * @in_fd: Input file pointer
 * @putback: Putback buffer ('\0' if empty)
 * @line_num: Line number
 */
struct gup_state {
    int in_fd;
    char putback;
    size_t line_num;
};

/*
 * Open a new GUP state
 *
 * @path: Path of input file
 * @res: Resulting state descriptor written here
 *
 * Returns zero on success
 */
int gup_open(const char *path, struct gup_state *res);

/*
 * Close a GUP state
 *
 * @state: GUP state to close
 */
void gup_close(struct gup_state *state);

#endif  /* !GUP_STATE_H */
