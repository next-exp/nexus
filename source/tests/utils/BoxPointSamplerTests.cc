#include "BoxPointSampler.h"
#include "RandomUtils.h"

#include <Randomize.hh>

#include <cmath>

#include <catch.hpp>

TEST_CASE("BoxPointSampler") {

  // This tests checks that the BoxPointSampler class generates vertices
  // in correct places. It does not test uniformity, for the moment.

  // The smaller dimension must be larger than twice the thickness
  // (which is chosen to be between 0 and 1).
  // The three dimensions are different by construction, to be general.
  auto a = 2 + 20 * G4UniformRand();
  auto b = a + 2;
  auto c = a + 3;

  for (G4int i=0; i<20; i++) {

    auto thick = G4UniformRand();
    auto sampler = nexus::BoxPointSampler(a/2., b/2., c/2., thick);
    auto vertex  = sampler.GenerateVertex(nexus::VOLUME);
    auto x = vertex.x();
    auto y = vertex.y();
    auto z = vertex.z();

    REQUIRE(std::abs(x) <=  a/2 + thick);
    REQUIRE(std::abs(y) <=  b/2 + thick);
    REQUIRE(std::abs(z) <=  c/2 + thick);

    if ((std::abs(x) < a/2) && (std::abs(y) < b/2)) {
      REQUIRE(std::abs(z) >= c/2);
      REQUIRE(std::abs(z) <= c/2 + thick);
    }

    if ((std::abs(x) < a/2) && (std::abs(z) < c/2)) {
      REQUIRE(std::abs(y) >= b/2);
      REQUIRE(std::abs(y) <= b/2 + thick);
    }

    if ((std::abs(y) < b/2) && (std::abs(z) < c/2)) {
      REQUIRE(std::abs(x) >= a/2);
      REQUIRE(std::abs(x) <= a/2 + thick);
    }
  }

  for (G4int i=0; i<20; i++) {

    auto sampler = nexus::BoxPointSampler(a/2., b/2., c/2., 0.);
    auto vertex  = sampler.GenerateVertex(nexus::INSIDE);
    auto x = vertex.x();
    auto y = vertex.y();
    auto z = vertex.z();

    REQUIRE(std::abs(x) <= a/2);
    REQUIRE(std::abs(y) <= b/2);
    REQUIRE(std::abs(z) <= c/2);

  }

}


TEST_CASE("Expected intersect") {
  // This test checks that the GetIntersect method
  // of BoxPointSampler produces the expected
  // intersect points for the simple cases of
  // a central point with directions parallel to
  // the faces of the box.
  auto inner_dim = 100;
  auto thickness =  10;
  auto sampler   = nexus::BoxPointSampler(inner_dim/2., inner_dim/2., inner_dim/2., thickness);
  auto origin    = G4ThreeVector(0., 0., 0.);

  G4double xdir[] = {1., 0., 0., -1., 0., 0.};
  G4double ydir[] = {0., 1., 0., 0., -1., 0.};
  G4double zdir[] = {0., 0., 1., 0., 0., -1.};
  for (G4int i=0; i<6; ++i){
    auto dir = G4ThreeVector(xdir[i], ydir[i], zdir[i]);
    auto intersect = sampler.GetIntersect(origin, dir);
    
    REQUIRE(intersect.x() == Approx(inner_dim * dir.x()));
    REQUIRE(intersect.y() == Approx(inner_dim * dir.y()));
    REQUIRE(intersect.z() == Approx(inner_dim * dir.z()));
  }

}

TEST_CASE("Box Arbitrary valid intersect") {
  // This test checks that for a sampler of arbitrary
  // position and arbitrary rotation a valid intersect
  // point is found for a randomly generated ray.
  auto inner_dim  = 100;
  auto thickness  =  10;

  auto origin  = G4ThreeVector(inner_dim * G4UniformRand(),
			       inner_dim * G4UniformRand(),
			       inner_dim * G4UniformRand());
  auto rotation = new G4RotationMatrix();
  rotation->rotateX(CLHEP::twopi * G4UniformRand());
  rotation->rotateY(CLHEP::twopi * G4UniformRand());
  rotation->rotateZ(CLHEP::twopi * G4UniformRand());
  
  auto sampler = nexus::BoxPointSampler(inner_dim/2., inner_dim/2., inner_dim/2.,
					thickness, origin, rotation);
  
  auto point = sampler.GenerateVertex(nexus::INSIDE);
  auto dir   = G4ThreeVector(G4UniformRand(),
			     G4UniformRand(),
			     G4UniformRand()).unit();
  
  auto intersect = sampler.GetIntersect(point, dir);
  
  // Check that the new ray passes through the original point.
  auto origin_point = point - intersect;
  REQUIRE(std::abs(origin_point.dot(dir)) == Approx(origin_point.mag()));

}
