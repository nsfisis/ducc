cat <<'EOF' > expected
1
1
2
3
EOF

test_diff <<'EOF'
int printf();

int main() {
#if 1
    printf("1\n");
#elif 1
    printf("2\n");
#else
    printf("3\n");
#endif

#if 1
    printf("1\n");
#elif 0
    printf("2\n");
#else
    printf("3\n");
#endif

#if 0
    printf("1\n");
#elif 1
    printf("2\n");
#else
    printf("3\n");
#endif

#if 0
    printf("1\n");
#elif 0
    printf("2\n");
#else
    printf("3\n");
#endif
}
EOF
