{
  lib,
  stdenv,
  pkgs,
  inputs,
  isDebug ? false,
  enableColor ? false,
  ...
}:

stdenv.mkDerivation rec {
  pname = "chlsdl";
  version = "0.0.1";

  src = pkgs.nix-gitignore.gitignoreSourcePure ''
    *

    !Makefile
    !config.mk

    !src
    !src/*.[ch]'' ./.;

  dontStrip = if isDebug then true else false;

  chlsdl-modules-pkg =
    if isDebug then
      inputs.chlsdl-modules.packages.${pkgs.stdenv.hostPlatform.system}.chlsdl-modules-debug
    else
      inputs.chlsdl-modules.packages.${pkgs.stdenv.hostPlatform.system}.default;

  buildInputs = with pkgs; [
    xorg.libX11
    libxmu
    xclip
    pcre2
    json_c
    (chlsdl-modules-pkg.override {
      enableColor = true;
    })
  ];

  buildPhase = lib.concatStringsSep " " (
    [
      "make"
      (if isDebug then "debug" else "release")
      "PREFIX=${chlsdl-modules-pkg}"
    ]
    ++ lib.optionals enableColor [ "COLOR=1" ]
    ++ lib.optionals isDebug [
      ";"
      "cp chlsdl-debug chlsdl"
    ]
  );

  installPhase = ''
    make install PREFIX=$out
  '';
  hardeningDisable = [ "all" ];
}
