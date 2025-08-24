#include "ast.h"
#include "cli.h"
#include "codegen.h"
#include "common.h"
#include "io.h"
#include "parse.h"
#include "preprocess.h"
#include "std.h"
#include "tokenize.h"

int main(int argc, char** argv) {
    CliArgs* cli_args = parse_cli_args(argc, argv);

    if (cli_args->totally_deligate_to_gcc) {
        return system(cli_args->gcc_command);
    }

    InFile* source = infile_open(cli_args->input_filename);
    TokenArray* pp_tokens = preprocess(source);
    TokenArray* tokens = tokenize(pp_tokens);
    Program* prog = parse(tokens);

    const char* assembly_filename;
    if (cli_args->output_assembly) {
        assembly_filename = cli_args->output_filename;
    } else {
        char* temp_filename = calloc(19, sizeof(char));
        temp_filename = strdup("/tmp/ducc-XXXXXX.s");
        mkstemps(temp_filename, strlen(".s"));
        assembly_filename = temp_filename;
    }
    FILE* assembly_file = assembly_filename ? fopen(assembly_filename, "wb") : stdout;
    codegen(prog, assembly_file);
    fclose(assembly_file);

    if (!cli_args->output_assembly) {
        char cmd_buf[256];
        if (cli_args->only_compile) {
            sprintf(cmd_buf, "gcc -c -s -o '%s' '%s'", cli_args->output_filename, assembly_filename);
        } else {
            sprintf(cmd_buf, "gcc -s -o '%s' '%s'", cli_args->output_filename, assembly_filename);
        }
        int result = system(cmd_buf);
        if (result != 0) {
            fatal_error("gcc failed: %d", result);
        }
    }
}
