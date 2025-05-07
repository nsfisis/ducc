set -e

cat <<'EOF' > expected
65
65
66
67
68
EOF
bash ../../test_diff.sh <<'EOF'
void* calloc();
int printf();

int main() {
    char* source = calloc(4, sizeof(char));

    source[0] = 'A';
    source[1] = 'B';
    source[2] = 'C';
    source[3] = 'D';

    int a = source[0];

    printf("%d\n", a);
    printf("%d\n", source[0]);
    printf("%d\n", source[1]);
    printf("%d\n", source[2]);
    printf("%d\n", source[3]);

    return 0;
}
EOF
