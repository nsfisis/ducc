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
// clang-format on

int main(int argc, char** argv) {
    if (argc == 1) {
        fatal_error("usage: ducc <FILE>");
    }
    InFile* source = infile_open(argv[1]);
    TokenArray* pp_tokens = preprocess(source);
    TokenArray* tokens = tokenize(pp_tokens);
    Program* prog = parse(tokens);
    codegen(prog);
}
