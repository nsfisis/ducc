{ pkgs, ... }:
{
  projectRootFile = "flake.nix";

  programs.nixfmt.enable = true;

  programs.clang-format.enable = true;
  # Exclude files formatted by hand.
  settings.formatter.clang-format.excludes = [ "examples/2048.c" ];
}
