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

  # pytest-instafail was unavailable in nixpkgs at time of writing
  mk-pytest-instafail = pypkgs:
    pypkgs.buildPythonPackage rec {
      pname = "pytest-instafail";
      version = "0.4.2";
      src = pypkgs.fetchPypi {
        inherit pname version;
        sha256 = "10lpr6mjcinabqynj6v85bvb1xmapnhqmg50nys1r6hg7zgky9qr";
      };
      buildInputs = [ pypkgs.pytest ];
    };

  # pytest-order was unavailable in nixpkgs at time of writing (not to be
  # confused with pytest-orderING, of which this is a fork)
  mk-pytest-order = pypkgs:
    pypkgs.buildPythonPackage rec {
      pname = "pytest-order";
      version = "0.9.3";
      src = pypkgs.fetchPypi {
        inherit pname version;
        sha256 = "1qd9zfpcbzm43knkg3ap22wssqabc2wn5ynlgg661xg6r6g6iy4k";
      };
      buildInputs = [ pypkgs.pytest ];
    };


  command = pkgs.writeShellScriptBin;

  mkPkgList = (ps: [
    ps.pandas
    ps.numpy
    ps.geant4
    ps.pytest
    ps.flaky
    ps.hypothesis
    ps.pytest_xdist
    (mk-pytest-instafail ps)
    (mk-pytest-order     ps)
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
