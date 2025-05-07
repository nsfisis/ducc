set -e

cat <<'EOF' > expected
EOF
bash ../../test_diff.sh <<'EOF'
int atoi(char*);
void* calloc(long, long);
void exit(int);
int getchar(void);
int isalnum(int);
int isalpha(int);
int isdigit(int);
int isspace(int);
void* memcpy(void*, void*, long);
int strcmp(char*, char*);
char* strstr(char*, char*);

int main() { return 0; }
EOF
