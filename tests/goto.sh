touch expected
test_diff <<'EOF'
int main() {
    goto end;
    return 1;
end:
    return 0;
}
EOF

cat <<'EOF' > expected
1
2
3
EOF
test_diff <<'EOF'
int printf(const char*, ...);

int main() {
    int i = 0;
loop:
    i++;
    printf("%d\n", i);
    if (i < 3)
        goto loop;
    return 0;
}
EOF

cat < /dev/null > expected
test_diff <<'EOF'
int main() {
    goto skip;
    int x = 5;
skip:
    return 0;
}
EOF

cat <<'EOF' > expected
start
middle
end
EOF
test_diff <<'EOF'
int printf(const char*, ...);

int main() {
    printf("start\n");
    goto middle;
first:
    printf("first\n");
    goto end;
middle:
    printf("middle\n");
    goto end;
last:
    printf("last\n");
end:
    printf("end\n");
    return 0;
}
EOF

cat <<'EOF' > expected
before
after
EOF
test_diff <<'EOF'
int printf(const char*, ...);

int main() {
    printf("before\n");
    {
        {
            goto out;
            printf("inside\n");
        }
        printf("middle\n");
    }
out:
    printf("after\n");
    return 0;
}
EOF

cat <<'EOF' > expected
x is 5
x is 10
EOF
test_diff <<'EOF'
int printf(const char*, ...);

int main() {
    int x = 5;
    if (x == 5) {
        printf("x is 5\n");
        goto next;
    }
    printf("x is not 5\n");
next:
    x = 10;
    printf("x is %d\n", x);
    return 0;
}
EOF

cat <<'EOF' > expected
case 2
done
EOF
test_diff <<'EOF'
int printf(const char*, ...);

int main() {
    int x = 2;
    switch (x) {
    case 1:
        printf("case 1\n");
        break;
    case 2:
        printf("case 2\n");
        goto done;
    case 3:
        printf("case 3\n");
        break;
    }
    printf("after switch\n");
done:
    printf("done\n");
    return 0;
}
EOF

# cat <<'EOF' > expected
# error: use of undeclared label 'undefined'
# EOF
# test_compile_error <<'EOF'
# int main() {
#     goto undefined;
#     return 0;
# }
# EOF

# cat <<'EOF' > expected
# error: redefinition of label 'duplicate'
# EOF
# test_compile_error <<'EOF'
# int main() {
# duplicate:
#     ;
# duplicate:
#     return 0;
# }
# EOF

# cat <<'EOF' > expected
# error: label at end of compound statement
# EOF
# test_compile_error <<'EOF'
# int main() {
#     {
#         goto end;
#     end:
#     }
#     return 0;
# }
# EOF
