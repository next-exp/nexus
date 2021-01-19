#include <RandomUtils.h>
#include <Randomize.hh>
#include "CLHEP/Units/SystemOfUnits.h"

#include <catch.hpp>

TEST_CASE("Direction Function") {

  // This tests checks that the RandomUtils:Direction class generates
  // vectors inside the given angles.

  for (G4int i=0; i<20; i++) {

    auto costheta_max = 2*(G4UniformRand()-0.5);
    auto costheta_min = 2*(G4UniformRand()-0.5);
    auto phi_max = 2*3.14*G4UniformRand();
    auto phi_min = 2*3.14*G4UniformRand();
    G4double x;
    G4double y;
    if (costheta_max < costheta_min) {
      x = costheta_max;
      costheta_max = costheta_min;
      costheta_min = x;
    }
    if (phi_max < phi_min) {
      y = phi_max;
      phi_max = phi_min;
      phi_min = y;
    }
    auto direction = nexus::Direction(costheta_min,costheta_max,phi_min,phi_max);
    G4double costheta_test = direction.z();
    G4double phi_test = asin(direction.y() / std::sin(acos(costheta_test)));

    REQUIRE(costheta_max >= costheta_test);
    REQUIRE(costheta_min <= costheta_test);
    REQUIRE(phi_max >= phi_test);
    REQUIRE(phi_min <= phi_test);
  }

}
