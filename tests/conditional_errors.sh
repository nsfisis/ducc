cat <<'EOF' > expected
main.c:2: expected '#endif', but got '<eof>'
EOF
test_compile_error <<'EOF'
#if 0
EOF

cat <<'EOF' > expected
main.c:3: expected '#endif', but got '<eof>'
EOF
test_compile_error <<'EOF'
#if 1
#else
EOF

cat <<'EOF' > expected
main.c:3: expected '#endif', but got '<eof>'
EOF
test_compile_error <<'EOF'
#if 1
#elif 1
EOF

cat <<'EOF' > expected
main.c:1: unexpected '#elif'; no corresponding '#if'*
EOF
test_compile_error <<'EOF'
#elif
#endif

int main() {}
EOF

cat <<'EOF' > expected
main.c:1: unexpected '#endif'; no corresponding '#if'*
EOF
test_compile_error <<'EOF'
#endif

int main() {}
EOF

cat <<'EOF' > expected
main.c:1: unexpected '#else'; no corresponding '#if'*
EOF
test_compile_error <<'EOF'
#else
#endif

int main() {}
EOF

cat <<'EOF' > expected
main.c:4: expected '#endif', but got '<eof>'
EOF
test_compile_error <<'EOF'
#if 1

int main() {}
EOF

cat <<'EOF' > a.h
#if 1
EOF
cat <<'EOF' > expected
./a.h:2: expected '#endif', but got '<eof>'
EOF
test_compile_error <<'EOF'
#include "a.h"
#endif

int main() {}
EOF
