cat <<'EOF' > expected
int printf ();

int main () {

 printf ( A is defined\n);


 printf ( A is undefined\n);
}
EOF

test_cpp <<'EOF'
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
