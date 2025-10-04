cat <<'EOF' > expected
argc = 4
argv[1] = hoge
argv[2] = piyo
argv[3] = fuga
EOF
test_diff hoge piyo fuga<<'EOF'
int printf();

int main(int argc, char** argv) {
    printf("argc = %d\n", argc);
    int i;
    for (i = 1; i < argc; ++i) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    return 0;
}
EOF

