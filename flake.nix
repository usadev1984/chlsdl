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
    }@inputs:
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

        packages = {
          default = self.packages.${system}.chlsdl;
          chlsdl-debug = self.packages.${system}.chlsdl.override {isDebug = true;};
          chlsdl = pkgs.callPackage ./package.nix {inherit inputs;};
        };
      }
    );
}
