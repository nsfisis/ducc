# Ducc

![Ducc logo](./assets/logo.svg)

Ducc, Decidedly Unimplemented C compiler, is a toy C compiler.


## Dependencies

* gcc
* make
* [just](https://github.com/casey/just)


### Optional: Nix support

This project includes Nix support with flake.

To build:

```
$ nix build
```

To develop:

```
$ nix develop
```


## Build

```
$ just build
$ just build 2
$ just build 3
```


## Test

```
$ just test
$ just test-self-hosted
$ just test-all
```


## License

See [LICENSE](./LICENSE).
