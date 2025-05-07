set -e

bash ../../test_exit_code.sh 10 <<'EOF'
int f(int a, int b, int c, int d, int e, int f) {
    return a;
}

int main() {
    return 10 * f(1, 2, 3, 4, 5, 6);
}
EOF

bash ../../test_exit_code.sh 20 <<'EOF'
int f(int a, int b, int c, int d, int e, int f) {
    return b;
}

int main() {
    return 10 * f(1, 2, 3, 4, 5, 6);
}
EOF

bash ../../test_exit_code.sh 30 <<'EOF'
int f(int a, int b, int c, int d, int e, int f) {
    return c;
}

int main() {
    return 10 * f(1, 2, 3, 4, 5, 6);
}
EOF

bash ../../test_exit_code.sh 40 <<'EOF'
int f(int a, int b, int c, int d, int e, int f) {
    return d;
}

int main() {
    return 10 * f(1, 2, 3, 4, 5, 6);
}
EOF

bash ../../test_exit_code.sh 50 <<'EOF'
int f(int a, int b, int c, int d, int e, int f) {
    return e;
}

int main() {
    return 10 * f(1, 2, 3, 4, 5, 6);
}
EOF

bash ../../test_exit_code.sh 60 <<'EOF'
int f(int a, int b, int c, int d, int e, int f) {
    return f;
}

int main() {
    return 10 * f(1, 2, 3, 4, 5, 6);
}
EOF
