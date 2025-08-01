// Currently the source code depends on the #include order.
// clang-format off
#include "std.h"
#include "common.c"
#include "io.c"
#include "sys.c"
#include "preprocess.c"
#include "tokenize.c"
#include "ast.c"
#include "parse.c"
#include "codegen.c"
// clang-format on

int main(int argc, char** argv) {
    if (argc == 1) {
        fatal_error("usage: ducc <FILE>");
    }
    InFile* source = read_all(argv[1]);
    PpToken* pp_tokens = preprocess(source);
    Token* tokens = tokenize(pp_tokens);
    Program* prog = parse(tokens);
    codegen(prog);
}
