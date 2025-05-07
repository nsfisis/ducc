set -e

cat <<'EOF' > expected
sizeof(int) = 4
sizeof(int*) = 8
sizeof(char) = 1
sizeof(char*) = 8
sizeof(long) = 8
sizeof(long*) = 8
sizeof(void*) = 8
sizeof(int**) = 8
sizeof(char**) = 8
sizeof(long**) = 8
sizeof(void**) = 8
sizeof(int***) = 8
sizeof(char***) = 8
sizeof(long***) = 8
sizeof(void***) = 8
EOF
bash ../../test_diff.sh <<'EOF'
int printf();

int main() {
    printf("sizeof(int) = %d\n", sizeof(int));
    printf("sizeof(int*) = %d\n", sizeof(int*));
    printf("sizeof(char) = %d\n", sizeof(char));
    printf("sizeof(char*) = %d\n", sizeof(char*));
    printf("sizeof(long) = %d\n", sizeof(long));
    printf("sizeof(long*) = %d\n", sizeof(long*));
    printf("sizeof(void*) = %d\n", sizeof(void*));
    printf("sizeof(int**) = %d\n", sizeof(int**));
    printf("sizeof(char**) = %d\n", sizeof(char**));
    printf("sizeof(long**) = %d\n", sizeof(long**));
    printf("sizeof(void**) = %d\n", sizeof(void**));
    printf("sizeof(int***) = %d\n", sizeof(int***));
    printf("sizeof(char***) = %d\n", sizeof(char***));
    printf("sizeof(long***) = %d\n", sizeof(long***));
    printf("sizeof(void***) = %d\n", sizeof(void***));
    return 0;
}
EOF
