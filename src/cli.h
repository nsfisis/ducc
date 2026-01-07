#ifndef DUCC_CLI_H
#define DUCC_CLI_H

#include "common.h"

typedef struct {
    const char* input_filename;
    const char* output_filename;
    bool output_assembly;
    bool only_compile;
    bool preprocess_only;
    bool generate_deps;
    bool totally_deligate_to_gcc;
    bool wasm;
    const char* gcc_command;
    StrArray include_dirs;
    StrArray defines;
} CliArgs;

CliArgs* parse_cli_args(int argc, char** argv);

#endif
