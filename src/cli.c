#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "version.h"

static void print_version() {
    printf("ducc v%s\n", DUCC_VERSION);
}

CliArgs* parse_cli_args(int argc, char** argv) {
    const char* output_filename = NULL;
    int positional_arguments_start = -1;
    bool opt_c = false;
    bool opt_E = false;
    bool opt_wasm = false;
    bool opt_MMD = false;
    StrArray include_dirs;
    strings_init(&include_dirs);

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            positional_arguments_start = i;
            break;
        }
        char c = argv[i][1];
        if (c == 'g') {
            // ignore
        } else if (c == 'O') {
            // ignore
        } else if (c == 'W') {
            // ignore
        } else if (c == 'M' && argv[i][2] == '\0') {
            // ignore -M
        } else if (c == 'I') {
            const char* dir = NULL;
            if (argv[i][2] != '\0') {
                // -Ipath format
                dir = argv[i] + 2;
            } else if (argc > i + 1) {
                // -I path format
                dir = argv[i + 1];
                ++i;
            } else {
                fatal_error("-I requires directory");
            }
            strings_push(&include_dirs, dir);
        } else if (c == 'o') {
            if (argc <= i + 1) {
                fatal_error("-o requires filename");
            }
            output_filename = argv[i + 1];
            ++i;
        } else if (c == 'c') {
            opt_c = true;
        } else if (c == 'E') {
            opt_E = true;
        } else if (strcmp(argv[i], "-MMD") == 0) {
            opt_MMD = true;
        } else if (strcmp(argv[i], "--version") == 0) {
            print_version();
            exit(0);
        } else if (strcmp(argv[i], "-std=gnu23") == 0) {
            // ignore -std=gnu23
        } else if (strcmp(argv[i], "--wasm") == 0) {
            opt_wasm = true;
        } else {
            fatal_error("unknown option: %s", argv[i]);
        }
    }
    if (positional_arguments_start == -1) {
        fatal_error("usage: ducc <file>");
    }

    CliArgs* a = calloc(1, sizeof(CliArgs));
    a->input_filename = argv[positional_arguments_start];
    a->output_filename = output_filename;
    a->output_assembly = !output_filename || str_ends_with(output_filename, ".s") || opt_wasm;
    a->only_compile = opt_c;
    a->preprocess_only = opt_E;
    a->totally_deligate_to_gcc = false;
    a->wasm = opt_wasm;
    a->gcc_command = NULL;
    a->generate_deps = opt_MMD;
    a->include_dirs = include_dirs;

    if (!a->only_compile && str_ends_with(a->input_filename, ".o")) {
        a->totally_deligate_to_gcc = true;
        StrBuilder builder;
        strbuilder_init(&builder);
        strbuilder_append_string(&builder, "gcc ");
        for (int i = 1; i < argc; ++i) {
            strbuilder_append_char(&builder, '\'');
            strbuilder_append_string(&builder, argv[i]);
            strbuilder_append_char(&builder, '\'');
            if (i != argc - 1) {
                strbuilder_append_char(&builder, ' ');
            }
        }
        a->gcc_command = builder.buf;
    }

    return a;
}
