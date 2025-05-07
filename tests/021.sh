set -e

cat <<'EOF' > expected
EOF
bash ../../test_diff.sh <<'EOF'
int main() {
    int a1;
    int* a2;
    char a3;
    char* a4;
    long a5;
    long* a6;
    void* a8;
    int** a10;
    char** a12;
    long** a14;
    void** a16;
    int*** a18;
    char*** a20;
    long*** a22;
    void*** a24;
    return 0;
}
EOF
