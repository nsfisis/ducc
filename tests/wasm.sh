cat > main.c <<'EOF'
int add(int a, int b) {
    return a + b;
}
EOF

"$ducc" "${CFLAGS:-}" --wasm -o main.wat main.c

wat2wasm main.wat -o main.wasm

cat > main.mjs <<'EOF'
import { readFile } from 'fs/promises';

const wasmBuffer = await readFile('./main.wasm');
const { instance } = await WebAssembly.instantiate(wasmBuffer);

console.log(instance.exports.add(3, 5));
EOF
node main.mjs > output

cat > expected <<'EOF'
8
EOF

diff -u expected output


cat > main.c <<'EOF'
int fib(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return fib(n - 1) + fib(n - 2);
    }
}
EOF

"$ducc" "${CFLAGS:-}" --wasm -o main.wat main.c

wat2wasm main.wat -o main.wasm

cat > main.mjs <<'EOF'
import { readFile } from 'fs/promises';

const wasmBuffer = await readFile('./main.wasm');
const { instance } = await WebAssembly.instantiate(wasmBuffer);

console.log(instance.exports.fib(10));
EOF
node main.mjs > output

cat > expected <<'EOF'
89
EOF

diff -u expected output
