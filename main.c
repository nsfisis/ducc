// Currently the source code depends on the #include order.
// clang-format off
#include "std.h"
#include "common.c"
#include "preprocess.c"
#include "tokenize.c"
#include "ast.c"
#include "parse.c"
#include "codegen.c"
#include "analyze.c"
// clang-format on

int main(int argc, char** argv) {
    if (argc == 1) {
        fatal_error("usage: ducc <FILE>");
    }
    FILE* in;
    if (strcmp(argv[1], "-") == 0) {
        in = stdin;
    } else {
        in = fopen(argv[1], "rb");
    }
    char* source = read_all(in);
    PpToken* pp_tokens = preprocess(source);
    Token* tokens = tokenize(pp_tokens);
    Program* prog = parse(tokens);
    analyze(prog);
    codegen(prog);
    return 0;
}
