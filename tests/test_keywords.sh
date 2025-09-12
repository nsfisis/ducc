cat <<'EOF' > expected


int printf ();
int main () {}
EOF

test_cpp <<'EOF'
// A keyword is treated as a normal identifier in preprocessing phase.
#define auto int

auto printf();
auto main() {}
EOF
