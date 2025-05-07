# P4Dcc

P4Dcc is a tiny, but self-hosted C compiler. It takes C source code and compiles it to assembly language. For assembling and linking, it deletegates to gcc as-is.

This project was started to prove the hypothesis: "Could a self-hostable C compiler be built in four days, if the feature set is carefully limited?" - "P4D" stands for the ISO 8601 notation meaning "four days." However, I actually completed the project by the morning of the third day.

The code is written following the instructions at https://www.sigbus.info/compilerbook, and several key design decisions were inspired by the book.


## Dependencies

* gcc
* [just](https://github.com/casey/just), a general-purpose task runner


## Build

```
$ just build
```


## Test

```
$ just test
$ just test-all  # test all things, including binary equiality between generations
```


## Design

To meet the four-day goal, many design decisions were made to reduce complexity (ideas directly taken from https://www.sigbus.info/compilerbook are not listed):

* Simplified declaration syntax
    * No support for `typedef`
        * Structs always begin with `struct` keyword
    * No support for array types
        * No stack-allocated arrays
        * All arrays are heap-allocated and accessed via pointers
    * No support for function types
    * Type information always precede the variable name
* Minimal syntax sugar
    * ~~No increment/decrement operators~~
        * Partially implemented after self-hosting
    * ~~No compound assignment operators~~
        * Implemented after self-hosting
    * ~~No `while`~~
        * Implemented after self-hosting
    * No `switch`
* Limited preprocessor
    * Supports only simple `#define` that replaces identifiers with single integer or identifier
* No global variables
    * Including `stdin`, `stdout`, and `stderr`
    * Only function/struct definitions/declarations are allowed at the top level
* No variable shadowing
    * All variables are function-scoped


## License

See [LICENSE](./LICENSE).
