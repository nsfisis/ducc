cat <<'EOF' > expected
int printf ();


int main () {

 printf ( __ducc__ is defined.\n);

 printf ( A is defined.\n);

 printf ( B is undefined.\n);


 return 0;
}
EOF

test_cpp <<'EOF'
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

cat <<'EOF' > expected
int printf ();


int main () {

 printf ( __ducc__ is defined.\n);

 printf ( A is defined.\n);

 printf ( B is undefined.\n);


 return 0;
}
EOF

test_cpp <<'EOF'
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
