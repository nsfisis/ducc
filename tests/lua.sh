LUA_REPO=https://github.com/lua/lua.git
LUA_TAG=v5.5.0

if [[ ! -d ../../lua ]]; then
    git clone --depth=1 --revision="$LUA_TAG" "$LUA_REPO" ../../lua
fi

cat > makefile.patch <<'EOF'
--- a/makefile
+++ b/makefile
@@ -76,8 +76,8 @@ MYLDFLAGS= -Wl,-E
 MYLIBS= -ldl


-CC= gcc
-CFLAGS= -Wall -O2 $(MYCFLAGS) -fno-stack-protector -fno-common -march=native
+CC= ../../build/ducc
+CFLAGS+= -Wall -O2 $(MYCFLAGS) -fno-stack-protector -fno-common -march=native
 AR= ar rc
 RANLIB= ranlib
 RM= rm -f
EOF

(
    cd ../../lua
    if ! git apply --reverse --check < ../tmp/lua/makefile.patch 2>/dev/null; then
        git apply < ../tmp/lua/makefile.patch
    fi
    make
)
