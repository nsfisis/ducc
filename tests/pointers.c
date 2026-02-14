#include <helpers.h>

void* calloc(long, long);

int glob_a;
int glob_b[12];

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
    int* const* const* a25;

    int x;
    int* y;
    y = &x;
    *y = 42;
    ASSERT_EQ(42, x);
    ASSERT_EQ(42, *y);

    char c;
    int ii;
    long l;
    c = 42;
    ii = 42 * 2;
    l = 42 * 3;

    char* cp1;
    char* cp2;
    int* ip1;
    int* ip2;
    long* lp1;
    long* lp2;

    cp1 = &c;
    cp2 = &c + 3;

    ip1 = &ii;
    ip2 = &ii + 3;

    lp1 = &l;
    lp2 = &l + 3;

    ASSERT_EQ(3, cp2 - cp1);
    ASSERT_EQ(3, ip2 - ip1);
    ASSERT_EQ(3, lp2 - lp1);

    int b;
    int* a = &b;
    a[0] = 42;
    ASSERT_EQ(42, *a);

    long* arr = calloc(10, sizeof(long));
    long i = 0;
    for (i = 0; i < 10; i = i + 1) {
        arr[i] = i;
    }
    for (i = 0; i < 10; i = i + 1) {
        ASSERT_EQ(i, *(arr + i));
        ASSERT_EQ(i, arr[i]);
    }

    char* source = calloc(4, sizeof(char));
    source[0] = 'A';
    source[1] = 'B';
    source[2] = 'C';
    source[3] = 'D';

    int ca = source[0];
    ASSERT_EQ(65, ca);
    ASSERT_EQ(65, source[0]);
    ASSERT_EQ(66, source[1]);
    ASSERT_EQ(67, source[2]);
    ASSERT_EQ(68, source[3]);

    int sa[10];
    for (int j = 0; j < 10; ++j) {
        sa[j] = j * j;
    }
    for (int j = 0; j < 10; ++j) {
        ASSERT_EQ(j * j, sa[j]);
    }

    ASSERT_EQ(0, glob_a);
    glob_a = 42;
    ASSERT_EQ(42, glob_a);
    ASSERT_EQ(48, sizeof(glob_b));
    for (int j = 0; j < 12; ++j) {
        ASSERT_EQ(0, glob_b[j]);
    }
    glob_b[11] = 123;
    ASSERT_EQ(123, glob_b[11]);

    int arr_a[10 * 10];
    int arr_b[10 + 10];
    int arr_c[1 << 2];
    ASSERT_EQ(400, sizeof(arr_a));
    ASSERT_EQ(80, sizeof(arr_b));
    ASSERT_EQ(16, sizeof(arr_c));

    "";
    "abc";
    "\"foo\"bar\\\n\"";

    ASSERT_EQ_STR("defghijkl", "def" "ghi"
    "jkl");

    char* h = " hello,world" + 1;
    ASSERT_EQ('h', *h);
    ASSERT_EQ('l', h[2]);
    ASSERT_EQ(',', *(h + 5));

    char* s = "hi";
    int results[3];
    int count = 0;
    while (*s++) {
        results[count++] = *s;
    }
    ASSERT_EQ(2, count);
    ASSERT_EQ(105, results[0]);
    ASSERT_EQ(0, results[1]);
}
