# To override the default python version:
#
#   nix-shell shell.nix --argstr py 39
#
{ py ? "38"
, nixpkgs-commit-id ? "15a64b2facc1b91f4361bdd101576e8886ef834b"
}:

# To update `nixpkgs-commit-id` go to https://status.nixos.org/, which lists the
# latest commit that passes all the tests for any release. Unless there is an
# overriding reason, pick the latest stable NixOS release, at the time of
# writing this is nixos-20.03.
let
  nixpkgs-url = "https://github.com/nixos/nixpkgs/archive/${nixpkgs-commit-id}.tar.gz";
  pkgs = import (builtins.fetchTarball { url = nixpkgs-url; }) {
    #overlays = [ (import ./nix/geant4.nix) ];
  };
  python = builtins.getAttr ("python" + py) pkgs;
  pypkgs = python.pkgs;

  command = pkgs.writeShellScriptBin;

  mkPkgList = (ps: [
    ps.pandas
    ps.numpy
    ps.geant4
    ps.pytest
    ps.flaky
    ps.hypothesis
    ps.pytest-xdist
    ps.pytest-instafail
    ps.pytest-order
  ]);

in

pkgs.mkShell rec {
  name = "nexus";
  buildInputs = (mkPkgList pypkgs) ++ [
    pkgs.git
    pkgs.scons
    pkgs.bear
    pkgs.clang_11
    pkgs.geant4
    pkgs.geant4.data.G4PhotonEvaporation
    pkgs.geant4.data.G4EMLOW
    pkgs.geant4.data.G4RadioactiveDecay
    pkgs.geant4.data.G4ENSDFSTATE
    pkgs.geant4.data.G4SAIDDATA
    pkgs.geant4.data.G4PARTICLEXS
    pkgs.geant4.data.G4NDL
    pkgs.root
    pkgs.gsl
    pkgs.hdf5
  ];

  HDF5_DIR = pkgs.symlinkJoin { name = "hdf5"; paths = [ pkgs.hdf5 pkgs.hdf5.dev ]; };
  HDF5_LIB = "${HDF5_DIR}/lib";
  HDF5_INC = "${HDF5_DIR}/include";

  shellHook = ''
    export NEXUSDIR=`pwd`
  '';

}
