struct CliArgs {
    const char* input_filename;
    const char* output_filename;
    BOOL output_executable;
};
typedef struct CliArgs CliArgs;

CliArgs* parse_cli_args(int argc, char** argv) {
    const char* output_filename = NULL;
    int positional_arguments_start = -1;

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
        } else if (c == 'o') {
            if (argc <= i + 1) {
                fatal_error("-o requires filename");
            }
            output_filename = argv[i + 1];
            ++i;
        }
    }
    if (positional_arguments_start == -1) {
        fatal_error("usage: ducc <file>");
    }

    CliArgs* a = calloc(1, sizeof(CliArgs));
    a->input_filename = argv[positional_arguments_start];
    a->output_filename = output_filename;
    a->output_executable = output_filename && !str_ends_with(output_filename, ".s");
    return a;
}
