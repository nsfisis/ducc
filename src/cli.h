#ifndef DUCC_CLI_H
#define DUCC_CLI_H

#include "std.h"

struct CliArgs {
    const char* input_filename;
    const char* output_filename;
    BOOL output_assembly;
    BOOL only_compile;
    BOOL generate_deps;
    BOOL totally_deligate_to_gcc;
    const char* gcc_command;
};
typedef struct CliArgs CliArgs;

CliArgs* parse_cli_args(int argc, char** argv);

#endif
