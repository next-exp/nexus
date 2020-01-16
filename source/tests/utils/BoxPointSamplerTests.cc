#include <BoxPointSampler.h>
#include <Randomize.hh>

#include <cmath>

#include <catch.hpp>

TEST_CASE("BoxPointSampler") {

  // This tests checks that the BoxPointSamplerr class generates vertices
  // in correct places. It does not test uniformity, for the moment.

  // The smaller dimension must be larger than twice the thickness
  // (which is chosen to be between 0 and 1).
  // The three dimensions are different by construction, to be general.
  auto a = 2 + 20 * G4UniformRand();
  auto b = a + 2;
  auto c = a + 3;

  for (G4int i=0; i<20; i++) {

    auto thick = G4UniformRand();
    auto sampler = nexus::BoxPointSampler(a, b, c, thick);
    auto vertex  = sampler.GenerateVertex("WHOLE_VOL");
    auto x = vertex.x();
    auto y = vertex.y();
    auto z = vertex.z();

    REQUIRE(x >= -a/2 - thick);
    REQUIRE(x <=  a/2 + thick);
    REQUIRE(y >= -b/2 - thick);
    REQUIRE(y <=  b/2 + thick);
    REQUIRE(z >= -c/2 - thick);
    REQUIRE(z <=  c/2 + thick);

    if ((std::abs(x) < a/2) & (std::abs(y) < b/2)) {
      REQUIRE(std::abs(z) >= c/2);
      REQUIRE(std::abs(z) <= c/2 + thick);
    }

    if ((std::abs(x) < a/2) & (std::abs(z) < c/2)) {
      REQUIRE(std::abs(y) >= b/2);
      REQUIRE(std::abs(y) <= b/2 + thick);
    }

    if ((std::abs(y) < b/2) & (std::abs(z) < c/2)) {
      REQUIRE(std::abs(x) >= a/2);
      REQUIRE(std::abs(x) <= a/2 + thick);
    }
  }

}
