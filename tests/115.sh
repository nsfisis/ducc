cat <<'EOF' > expected
1
1
1
1
1
2
2
2
2
2
2
2
2
2
2
2
2
2
2
2
2
2
2
2
4
4
4
4
4
4
4
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
8
EOF

test_diff <<'EOF'
int printf();

int main() {
    printf("%d\n", sizeof(char));
    printf("%d\n", sizeof(signed char));
    printf("%d\n", sizeof(char signed));
    printf("%d\n", sizeof(unsigned char));
    printf("%d\n", sizeof(char unsigned));

    printf("%d\n", sizeof(short));
    printf("%d\n", sizeof(signed short));
    printf("%d\n", sizeof(short signed));
    printf("%d\n", sizeof(short int));
    printf("%d\n", sizeof(int short));
    printf("%d\n", sizeof(signed short int));
    printf("%d\n", sizeof(signed int short));
    printf("%d\n", sizeof(short signed int));
    printf("%d\n", sizeof(short int signed));
    printf("%d\n", sizeof(int short signed));
    printf("%d\n", sizeof(int signed short));

    printf("%d\n", sizeof(unsigned short));
    printf("%d\n", sizeof(short unsigned));
    printf("%d\n", sizeof(unsigned short int));
    printf("%d\n", sizeof(unsigned int short));
    printf("%d\n", sizeof(short unsigned int));
    printf("%d\n", sizeof(short int unsigned));
    printf("%d\n", sizeof(int short unsigned));
    printf("%d\n", sizeof(int unsigned short));

    printf("%d\n", sizeof(int));
    printf("%d\n", sizeof(signed));
    printf("%d\n", sizeof(signed int));
    printf("%d\n", sizeof(int signed));

    printf("%d\n", sizeof(unsigned));
    printf("%d\n", sizeof(unsigned int));
    printf("%d\n", sizeof(int unsigned));

    printf("%d\n", sizeof(long));
    printf("%d\n", sizeof(signed long));
    printf("%d\n", sizeof(long signed));
    printf("%d\n", sizeof(long int));
    printf("%d\n", sizeof(int long));
    printf("%d\n", sizeof(signed long int));
    printf("%d\n", sizeof(signed int long));
    printf("%d\n", sizeof(long signed int));
    printf("%d\n", sizeof(long int signed));
    printf("%d\n", sizeof(int long signed));
    printf("%d\n", sizeof(int signed long));

    printf("%d\n", sizeof(unsigned long));
    printf("%d\n", sizeof(long unsigned));
    printf("%d\n", sizeof(unsigned long int));
    printf("%d\n", sizeof(unsigned int long));
    printf("%d\n", sizeof(long unsigned int));
    printf("%d\n", sizeof(long int unsigned));
    printf("%d\n", sizeof(int long unsigned));
    printf("%d\n", sizeof(int unsigned long));

    printf("%d\n", sizeof(long long));
    printf("%d\n", sizeof(signed long long));
    printf("%d\n", sizeof(long signed long));
    printf("%d\n", sizeof(long long signed));
    printf("%d\n", sizeof(long long int));
    printf("%d\n", sizeof(long int long));
    printf("%d\n", sizeof(int long long));
    printf("%d\n", sizeof(signed long long int));
    printf("%d\n", sizeof(signed long int long));
    printf("%d\n", sizeof(signed int long long));
    printf("%d\n", sizeof(long signed long int));
    printf("%d\n", sizeof(long signed int long));
    printf("%d\n", sizeof(int signed long long));
    printf("%d\n", sizeof(long long signed int));
    printf("%d\n", sizeof(long int signed long));
    printf("%d\n", sizeof(int long signed long));
    printf("%d\n", sizeof(long long int signed));
    printf("%d\n", sizeof(long int long signed));
    printf("%d\n", sizeof(int long long signed));

    printf("%d\n", sizeof(unsigned long long));
    printf("%d\n", sizeof(long unsigned long));
    printf("%d\n", sizeof(long long unsigned));
    printf("%d\n", sizeof(unsigned long long int));
    printf("%d\n", sizeof(unsigned long int long));
    printf("%d\n", sizeof(unsigned int long long));
    printf("%d\n", sizeof(long unsigned long int));
    printf("%d\n", sizeof(long unsigned int long));
    printf("%d\n", sizeof(int unsigned long long));
    printf("%d\n", sizeof(long long unsigned int));
    printf("%d\n", sizeof(long int unsigned long));
    printf("%d\n", sizeof(int long unsigned long));
    printf("%d\n", sizeof(long long int unsigned));
    printf("%d\n", sizeof(long int long unsigned));
    printf("%d\n", sizeof(int long long unsigned));
}
EOF
