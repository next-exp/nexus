self: super: {
  geant4 = super.geant4.overrideAttrs (oldAttrs: rec {
    version = "10.6.3";

    src = super.fetchurl {
    url = "https://geant4-data.web.cern.ch/geant4-data/releases/geant4.10.06.p03.tar.gz";
    sha256 = "1wzv5xky1pfm7wdfdkvqcaaqlcnsrz35dc7zcrxh8l3j5rki6pqb";
    };

  });
}
