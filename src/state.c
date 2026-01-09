/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <stdio.h>
#include <stddef.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "gup/state.h"

int
gup_open(const char *path, struct gup_state *res)
{
    if (path == NULL || res == NULL) {
        errno = -EINVAL;
        return -1;
    }

    memset(res, 0, sizeof(*res));
    res->in_fd = open(path, O_RDONLY);
    if (res->in_fd < 0) {
        return -1;
    }

    res->line_num = 1;
    if ((res->out_fp = fopen(ASMOUT_DEFAULT, "w")) == NULL) {
        close(res->in_fd);
        return -1;
    }
    return 0;
}

void
gup_close(struct gup_state *state)
{
    if (state == NULL) {
        return;
    }

    close(state->in_fd);
    fclose(state->out_fp);
}
