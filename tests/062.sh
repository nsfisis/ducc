set -e

cat <<'EOF' > expected
__ducc__ is defined.
A is defined.
B is undefined.
EOF

bash ../../test_diff.sh <<'EOF'
int printf();

#define A 123

int main() {

#ifdef __ducc__
    printf("__ducc__ is defined.\n");
#else
    printf("__ducc__ is undefined.\n");
#endif

#ifdef A
    printf("A is defined.\n");
#else
    printf("A is undefined.\n");
#endif

#ifdef B
    printf("B is defined.\n");
#else
    printf("B is undefined.\n");
#endif

#define B 456

    return 0;
}
EOF
