{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-25.11";
    # nixpkgs-unstable.url = "github:nixos/nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
    chlsdl-modules.url = "github:usadev1984/chlsdl-modules";
  };
  outputs =
    {
      self,
      nixpkgs,
      # nixpkgs-unstable,
      utils,
      chlsdl-modules,
      ...
    }:
    utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        # pkgs-unstable = nixpkgs-unstable.legacyPackages.${system};
      in
      {
        devShell = pkgs.mkShell {
          nativeBuildInputs = [
            pkgs.clang-tools
            pkgs.clang
            pkgs.bear
            pkgs.pkg-config
            pkgs.gcc
          ];
          buildInputs = with pkgs; [
            xorg.libX11
            libxmu
            xclip
            pcre2
            json_c
            chlsdl-modules.packages.${system}.default
          ];
          hardeningDisable = [ "all" ];
        };
        packages.default = pkgs.stdenv.mkDerivation rec {
          pname = "chlsdl";
          version = "0.0.1";

          src = pkgs.nix-gitignore.gitignoreSourcePure ''
            *

            !Makefile
            !config.mk

            !src
            !src/*.[ch]'' ./.;

          buildInputs = with pkgs; [
            xorg.libX11
            libxmu
            xclip
            pcre2
            json_c
            chlsdl-modules.packages.${system}.default
          ];

          buildPhase = ''
            make -j$((`nproc`+1)) release PREFIX=${chlsdl-modules.packages.${system}.default} COLOR=1
          '';
          installPhase = ''
            make install PREFIX=$out
          '';
          hardeningDisable = [ "all" ];
        };
      }
    );
}
