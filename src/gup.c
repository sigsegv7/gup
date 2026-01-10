/*
 * Copyright (C) 2026, Ian Moffett.
 * Provided under the BSD-3 clause.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "gup/state.h"
#include "gup/parser.h"

#define GUP_VERSION "0.0.3"
#define ELAPSED_NS(STARTP, ENDP)                            \
    (double)((ENDP)->tv_sec - (STARTP)->tv_sec) * 1.0e9 +    \
        (double)((ENDP)->tv_nsec - (STARTP)->tv_nsec)

static bool asm_only = false;
static const char *bin_fmt = "elf64";

static void
help(void)
{
    printf(
        "the gup compiler - gup!\n"
        "-----------------------------\n"
        "[-h]   Display this help menu\n"
        "[-v]   Display the version\n"
    );
}

static void
version(void)
{
    printf(
        "gup version %s\n"
        "Copyright (c) 2026, Ian Moffett\n",
        GUP_VERSION);
}

static int
compile(const char *path)
{
    char cmd[32];
    struct gup_state state;
    struct timespec start, end;
    double elapsed_ms, elapsed_ns;

    if (gup_open(path, &state) < 0) {
        perror("gup_open");
        printf("fatal: failed to open \"%s\"\n", path);
        return -1;
    }

    clock_gettime(CLOCK_REALTIME, &start);
    if (gup_parse(&state) < 0) {
        printf("fatal: failed to parse \"%s\"\n", path);
        gup_close(&state);
        return -1;
    }

    clock_gettime(CLOCK_REALTIME, &end);
    elapsed_ns = ELAPSED_NS(&start, &end);
    elapsed_ms = elapsed_ns / 1e+6;

    printf("compiled in %.2fms [%.2fns]\n", elapsed_ms, elapsed_ns);
    gup_close(&state);

    /* Generate the output if we can */
    if (!asm_only) {
        snprintf(
            cmd,
            sizeof(cmd),
            "nasm -f%s gupgen.asm",
            bin_fmt
        );

        system(cmd);
        remove("gupgen.asm");
    }
    return 0;
}

int
main(int argc, char **argv)
{
    int opt;

    while ((opt = getopt(argc, argv, "hvaf:")) != -1) {
        switch (opt) {
        case 'h':
            help();
            break;
        case 'v':
            version();
            break;
        case 'a':
            asm_only = true;
            break;
        case 'f':
            bin_fmt = strdup(optarg);
            break;
        }
    }

    while (optind < argc) {
        if (compile(argv[optind++]) < 0) {
            break;
        }
    }

    return 0;
}
