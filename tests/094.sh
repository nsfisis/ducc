set -e

touch expected
bash ../../test_diff.sh <<'EOF'
// A keyword is treated as a normal identifier in preprocessing phase.
#define auto int

auto printf();
auto main() {}
EOF
