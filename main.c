// Currently the source code depends on the #include order.
// clang-format off
#include "src/std.h"
#include "src/common.c"
#include "src/io.c"
#include "src/sys.c"
#include "src/preprocess.c"
#include "src/tokenize.c"
#include "src/ast.c"
#include "src/parse.c"
#include "src/codegen.c"
#include "src/cli.c"
// clang-format on

int main(int argc, char** argv) {
    CliArgs* cli_args = parse_cli_args(argc, argv);
    InFile* source = infile_open(cli_args->input_filename);
    TokenArray* pp_tokens = preprocess(source);
    TokenArray* tokens = tokenize(pp_tokens);
    Program* prog = parse(tokens);

    const char* assembly_filename;
    if (cli_args->output_executable) {
        char* temp_filename = calloc(19, sizeof(char));
        temp_filename = strdup("/tmp/ducc-XXXXXX.s");
        mkstemps(temp_filename, strlen(".s"));
        assembly_filename = temp_filename;
    } else {
        assembly_filename = cli_args->output_filename;
    }
    FILE* assembly_file = assembly_filename ? fopen(assembly_filename, "wb") : stdout;
    codegen(prog, assembly_file);
    fclose(assembly_file);

    if (cli_args->output_executable) {
        char cmd_buf[256];
        sprintf(cmd_buf, "gcc -s -o '%s' '%s'", cli_args->output_filename, assembly_filename);
        int result = system(cmd_buf);
        if (result != 0) {
            fatal_error("gcc failed: %d", result);
        }
    }
}
