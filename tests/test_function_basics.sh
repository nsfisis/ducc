test_exit_code 66 <<'EOF'
int foo() {
    int i;
    int ret;
    i = 0;
    ret = 0;
    for (i = 0; i < 100; i = i + 1) {
        if (i == 12) {
            break;
        }
        ret = ret + i;
    }
    return ret;
}

int main() {
    return foo();
}
EOF

test_exit_code 10 <<'EOF'
int f(int a, int b, int c, int d, int e, int f) {
    return a;
}

int main() {
    return 10 * f(1, 2, 3, 4, 5, 6);
}
EOF

test_exit_code 20 <<'EOF'
int f(int a, int b, int c, int d, int e, int f) {
    return b;
}

int main() {
    return 10 * f(1, 2, 3, 4, 5, 6);
}
EOF

test_exit_code 30 <<'EOF'
int f(int a, int b, int c, int d, int e, int f) {
    return c;
}

int main() {
    return 10 * f(1, 2, 3, 4, 5, 6);
}
EOF

test_exit_code 40 <<'EOF'
int f(int a, int b, int c, int d, int e, int f) {
    return d;
}

int main() {
    return 10 * f(1, 2, 3, 4, 5, 6);
}
EOF

test_exit_code 50 <<'EOF'
int f(int a, int b, int c, int d, int e, int f) {
    return e;
}

int main() {
    return 10 * f(1, 2, 3, 4, 5, 6);
}
EOF

test_exit_code 60 <<'EOF'
int f(int a, int b, int c, int d, int e, int f) {
    return f;
}

int main() {
    return 10 * f(1, 2, 3, 4, 5, 6);
}
EOF

touch expected
test_diff <<'EOF'
int main() {
    return 0;
}
EOF

