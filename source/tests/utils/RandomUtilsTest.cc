#include <RandomUtils.h>
#include <Randomize.hh>
#include <G4RandomDirection.hh>
#include "CLHEP/Units/SystemOfUnits.h"

#include <catch.hpp>
#include <iostream>
#include <cmath>
using namespace std;

TEST_CASE("Direction Function") {

  // This tests checks that the RandomUtils:Direction class generates
  // vectors inside the given angles.

  for (G4int i=0; i<20; i++) {

    auto costheta_max = 2*(G4UniformRand()-0.5);
    auto costheta_min = 2*(G4UniformRand()-0.5);
    auto phi_max = 2*CLHEP::pi*G4UniformRand();
    auto phi_min = 2*CLHEP::pi*G4UniformRand();

    if (costheta_max < costheta_min) {
      std::swap(costheta_max,costheta_min);
    }
    if (phi_max < phi_min) {
      std::swap(phi_max,phi_min);
    }

    G4ThreeVector direction = nexus::RandomDirectionInRange(costheta_min,costheta_max,phi_min,phi_max);
    G4double costheta_test = direction.z();
    G4double phi_test = std::atan2(direction.y(), direction.x());

    if (phi_test < 0){
      phi_test += 2*CLHEP::pi;
    }

    REQUIRE(costheta_max >= costheta_test);
    REQUIRE(costheta_min <= costheta_test);
    REQUIRE(phi_max >= phi_test);
    REQUIRE(phi_min <= phi_test);
  }

}


TEST_CASE("Polarization Function") {

  // RandomPolarization gives a unit vector perpendicular to the direction,
  // uniform in azimuth around it: along any fixed axis perpendicular to the
  // direction, the squared projection averages 1/2.

  std::vector<G4ThreeVector> directions = {G4ThreeVector(1, 0, 0), G4ThreeVector(0, 1, 0),
                                           G4ThreeVector(0, 0, 1), G4ThreeVector(0, 0, -1)};
  for (G4int i=0; i<6; i++) directions.push_back(G4RandomDirection());

  const G4int n = 100000;
  for (auto dir : directions) {
    G4ThreeVector axis = dir.cross(G4RandomDirection()).unit();
    G4double sum = 0.;
    for (G4int i=0; i<n; i++) {
      G4ThreeVector pol = nexus::RandomPolarization(dir);
      REQUIRE(std::abs(pol.mag() - 1.) < 1e-12);
      REQUIRE(std::abs(pol.dot(dir)) < 1e-12);
      sum += std::pow(pol.dot(axis), 2);
    }
    REQUIRE(std::abs(sum / n - 0.5) < 0.006);
  }

}
