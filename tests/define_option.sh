cat <<'EOF' > expected
42
EOF
cat <<'EOF' > main.c
int printf(const char*, ...);

int main() {
#ifdef FOO
    printf("%d\n", 42);
#else
    printf("%d\n", 0);
#endif
}
EOF
"$ducc" -DFOO -o a.out main.c
./a.out > output
diff -u expected output

cat <<'EOF' > expected
100
EOF
cat <<'EOF' > main.c
int printf(const char*, ...);

int main() {
    printf("%d\n", FOO);
}
EOF
"$ducc" -DFOO=100 -o a.out main.c
./a.out > output
diff -u expected output

cat <<'EOF' > expected
10,5
EOF
cat <<'EOF' > main.c
int printf(const char*, ...);

int main() {
    printf("%d,%d\n", A, B);
}
EOF
"$ducc" -DA=10 -DB=5 -o a.out main.c
./a.out > output
diff -u expected output

cat <<'EOF' > expected
1
EOF
cat <<'EOF' > main.c
int printf(const char*, ...);

int main() {
    printf("%d\n", FEATURE);
}
EOF
"$ducc" -DFEATURE -o a.out main.c
./a.out > output
diff -u expected output

cat <<'EOF' > expected
1,2
EOF
cat <<'EOF' > main.c
int printf(const char*, ...);

int main() {
    printf("%d,%d\n", A, B);
}
EOF
"$ducc" -D A -D B=2 -o a.out main.c
./a.out > output
diff -u expected output
