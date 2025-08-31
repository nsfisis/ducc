{
  description = "Decidedly Unimplemented C compiler, a toy C compiler";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";

    systems.url = "github:nix-systems/x86_64-linux";

    flake-utils = {
      url = "github:numtide/flake-utils";
      inputs.systems.follows = "systems";
    };

    treefmt-nix = {
      url = "github:numtide/treefmt-nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
      treefmt-nix,
      ...
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
        treefmt = treefmt-nix.lib.evalModule pkgs ./treefmt.nix;
      in
      {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "ducc";
          version = "0.1.0";
          src = ./.;
          installPhase = ''
            mkdir -p $out/bin
            cp build/ducc $out/bin
          '';
        };

        devShells.default = pkgs.mkShell {
          packages = [
            pkgs.just
          ];
        };

        formatter = treefmt.config.build.wrapper;
      }
    );
}
