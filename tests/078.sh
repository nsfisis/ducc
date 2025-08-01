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

#ifndef __ducc__
    printf("__ducc__ is undefined.\n");
#else
    printf("__ducc__ is defined.\n");
#endif

#ifndef A
    printf("A is undefined.\n");
#else
    printf("A is defined.\n");
#endif

#ifndef B
    printf("B is undefined.\n");
#else
    printf("B is defined.\n");
#endif

#define B 456

    return 0;
}
EOF
