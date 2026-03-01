{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-25.11";
    # nixpkgs-unstable.url = "github:nixos/nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
  };
  outputs =
    {
      self,
      nixpkgs,
      # nixpkgs-unstable,
      utils,
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
            xorg.libX11.dev
            libxmu.dev
            pcre2.dev
          ];
          hardeningDisable = [ "all" ];
        };
        packages.default = pkgs.stdenv.mkDerivation rec {
          pname = "chlsdl";
          version = "0.0.1";

          src = ./.;

          buildInputs = with pkgs; [
            xorg.libX11.dev
            libxmu.dev
            pcre2.dev
          ];

          buildPhase = ''
            make -B libchlsdl-common COLOR=1
            make -B release COLOR=1
          '';
          installPhase = ''
            make install PREFIX=$out
          '';
          hardeningDisable = [ "all" ];
        };
      }
    );
}
