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

cat <<'EOF' > expected
1
1
2
2
3
4
3
4
EOF

test_diff <<'EOF'
int printf();

int main() {
#if 1
# if 1
    printf("1\n");
# else
    printf("2\n");
# endif
#else
# if 1
    printf("3\n");
# else
    printf("4\n");
# endif
#endif

#if 1
# if 1
    printf("1\n");
# else
    printf("2\n");
# endif
#else
# if 0
    printf("3\n");
# else
    printf("4\n");
# endif
#endif

#if 1
# if 0
    printf("1\n");
# else
    printf("2\n");
# endif
#else
# if 1
    printf("3\n");
# else
    printf("4\n");
# endif
#endif

#if 1
# if 0
    printf("1\n");
# else
    printf("2\n");
# endif
#else
# if 0
    printf("3\n");
# else
    printf("4\n");
# endif
#endif

#if 0
# if 1
    printf("1\n");
# else
    printf("2\n");
# endif
#else
# if 1
    printf("3\n");
# else
    printf("4\n");
# endif
#endif

#if 0
# if 1
    printf("1\n");
# else
    printf("2\n");
# endif
#else
# if 0
    printf("3\n");
# else
    printf("4\n");
# endif
#endif

#if 0
# if 0
    printf("1\n");
# else
    printf("2\n");
# endif
#else
# if 1
    printf("3\n");
# else
    printf("4\n");
# endif
#endif

#if 0
# if 0
    printf("1\n");
# else
    printf("2\n");
# endif
#else
# if 0
    printf("3\n");
# else
    printf("4\n");
# endif
#endif
}
EOF

