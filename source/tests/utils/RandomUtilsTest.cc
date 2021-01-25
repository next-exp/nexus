#include <RandomUtils.h>
#include <Randomize.hh>
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
    //auto phi_max = CLHEP::pi*G4UniformRand();
    //auto phi_min = CLHEP::pi*G4UniformRand();

    if (costheta_max < costheta_min) {
      std::swap(costheta_max,costheta_min);
    }
    if (phi_max < phi_min) {
      std::swap(phi_max,phi_min);
    }

    //auto direction = nexus::Direction(costheta_min,costheta_max,phi_min,phi_max);
    G4ThreeVector direction = nexus::Direction(costheta_min,costheta_max,phi_min,phi_max);
    G4double costheta_test = direction.z();
    G4double phi_test = std::atan2(direction.y(), direction.x());

    REQUIRE(costheta_max >= costheta_test);
    REQUIRE(costheta_min <= costheta_test);
    REQUIRE(phi_max >= phi_test);
    REQUIRE(phi_min <= phi_test);
  }

}
