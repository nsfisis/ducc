cat <<'EOF' > expected
EOF
test_diff <<'EOF'
typedef long size_t;

int atoi(const char*);
void* calloc(size_t, size_t);
void exit(int);
int getchar(void);
int isalnum(int);
int isalpha(int);
int isdigit(int);
int isspace(int);
void* memcpy(void*, void*, size_t);
int printf(const char*, ...);
int sprintf(char*, const char*, ...);
int strcmp(const char*, const char*);
char* strstr(const char*, const char*);

int main() { return 0; }
EOF

