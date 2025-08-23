cat <<'EOF' > input
abc
EOF
cat <<'EOF' > expected
4
abc
EOF
test_diff <<'EOF'
void* calloc();
int getchar();
int printf();

int read_all(char* buf) {
    int c;
    int n = 0;
    for (0; 1; 0) {
        c = getchar();
        if (c == -1) {
            break;
        }
        buf[n] = c;
        n = n + 1;
    }
    return n;
}

int main() {
    char* source = calloc(1024, sizeof(char));
    int source_len = read_all(source);

    printf("%d\n", source_len);
    printf("%s", source);

    return 0;
}
EOF
