#include "cli.h"
#include "common.h"

CliArgs* parse_cli_args(int argc, char** argv) {
    const char* output_filename = NULL;
    int positional_arguments_start = -1;
    BOOL only_compile = FALSE;

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
        } else if (c == 'M') {
            // ignore
        } else if (c == 'o') {
            if (argc <= i + 1) {
                fatal_error("-o requires filename");
            }
            output_filename = argv[i + 1];
            ++i;
        } else if (c == 'c') {
            only_compile = TRUE;
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
    a->output_assembly = !output_filename || str_ends_with(output_filename, ".s");
    a->only_compile = only_compile;
    a->totally_deligate_to_gcc = FALSE;
    a->gcc_command = NULL;

    if (!a->only_compile && str_ends_with(a->input_filename, ".o")) {
        a->totally_deligate_to_gcc = TRUE;
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
