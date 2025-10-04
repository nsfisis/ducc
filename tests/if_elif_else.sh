cat <<'EOF' > expected
int printf ();


int main () {
 printf ( 1\n);
}
EOF

test_cpp <<'EOF'
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
int printf ();

int main () {
 printf ( 1\n);

 printf ( 1\n);

 printf ( 2\n);

 printf ( 3\n);
}
EOF

test_cpp <<'EOF'
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
int printf ();

int main () {
 printf ( 1\n);

 printf ( 1\n);

 printf ( 2\n);

 printf ( 2\n);

 printf ( 3\n);

 printf ( 4\n);

 printf ( 3\n);

 printf ( 4\n);
}
EOF

test_cpp <<'EOF'
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
