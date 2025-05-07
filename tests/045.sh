set -e

cat <<'EOF' > expected
EOF
bash ../../test_diff.sh <<'EOF'
int atoi(const char*);
void* calloc(long, long);
void exit(int);
int getchar(void);
int isalnum(int);
int isalpha(int);
int isdigit(int);
int isspace(int);
void* memcpy(void*, void*, long);
int printf();
int sprintf();
int strcmp(const char*, const char*);
char* strstr(const char*, const char*);

int main() { return 0; }
EOF
