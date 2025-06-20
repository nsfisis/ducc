set -e

cat <<'EOF' > expected
hi
hello 246
EOF
bash ../../test_diff.sh <<'EOF'
i\
n\
t ma\
in() {
    pri\
ntf("hi\
\nhello %d\n", 1\
2\
3 <\
< 1);
    re\
turn 0;
}
EOF
