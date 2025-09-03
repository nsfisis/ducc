cat <<'EOF' > expected
A is defined
A is undefined
EOF

test_diff <<'EOF'
int printf();

int main() {
#define A 1

#ifdef A
    printf("A is defined\n");
#else
    printf("A is undefined\n");
#endif

#undef A

#ifdef A
    printf("A is defined\n");
#else
    printf("A is undefined\n");
#endif
}
EOF

