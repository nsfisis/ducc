cat <<'EOF' > expected
1
EOF

test_diff <<'EOF'
int printf();

#define A
#define B

int main() {
#if defined A && defined(B)
    printf("1\n");
#else
    printf("2\n");
#endif
}
EOF
