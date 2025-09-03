#ifndef DUCC_CLI_H
#define DUCC_CLI_H

#include "std.h"

typedef struct {
    const char* input_filename;
    const char* output_filename;
    bool output_assembly;
    bool only_compile;
    bool generate_deps;
    bool totally_deligate_to_gcc;
    const char* gcc_command;
} CliArgs;

CliArgs* parse_cli_args(int argc, char** argv);

#endif
