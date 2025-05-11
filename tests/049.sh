set -e

cat <<'EOF' > input
foo
bar
baz
EOF
cat <<'EOF' > expected
foo
foo
bar
bar
baz
baz
EOF
bash ../../test_diff.sh hoge piyo fuga<<'EOF'
struct FILE;
typedef struct FILE FILE;
extern FILE* stdin;
extern FILE* stdout;
int fprintf();
char* fgets();
void* calloc();

int main() {
    char* buf = calloc(256, sizeof(char));
    while (fgets(buf, 256, stdin)) {
        fprintf(stdout, "%s%s", buf, buf);
    }
    return 0;
}
EOF
