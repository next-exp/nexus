#include <CylinderPointSampler2020.h>
#include <Randomize.hh>

#include <catch.hpp>

TEST_CASE("Barrel and Caps intersection") {
  // Tests that rays from the origin parallel
  // to the axes are found to intersect with the
  // Cylinder sampler at the expected points.
  auto minRad  = 200;
  auto maxRad  = 220;
  auto halfLen = 500;
  auto sampler = nexus::CylinderPointSampler2020(minRad, maxRad, halfLen);
  auto origin  = G4ThreeVector(0., 0., 0.);

  // Check caps.
  auto intersect = sampler.GetIntersect(origin, G4ThreeVector(0., 0., 1.));
  REQUIRE(intersect.x() == Approx(0.));
  REQUIRE(intersect.y() == Approx(0.));
  REQUIRE(intersect.z() == Approx(halfLen));
  intersect = sampler.GetIntersect(origin, G4ThreeVector(0., 0., -1.));
  REQUIRE(intersect.x() == Approx(0.));
  REQUIRE(intersect.y() == Approx(0.));
  REQUIRE(intersect.z() == Approx(-halfLen));

  // Now the barrel.
  intersect = sampler.GetIntersect(origin, G4ThreeVector(1., 0., 0.));
  REQUIRE(intersect.x() == Approx(minRad));
  REQUIRE(intersect.y() == Approx(0.));
  REQUIRE(intersect.z() == Approx(0.));
  intersect = sampler.GetIntersect(origin, G4ThreeVector(-1., 0., 0.));
  REQUIRE(intersect.x() == Approx(-minRad));
  REQUIRE(intersect.y() == Approx(0.));
  REQUIRE(intersect.z() == Approx(0.));
  intersect = sampler.GetIntersect(origin, G4ThreeVector(0., 1., 0.));
  REQUIRE(intersect.x() == Approx(0.));
  REQUIRE(intersect.y() == Approx(minRad));
  REQUIRE(intersect.z() == Approx(0.));
  intersect = sampler.GetIntersect(origin, G4ThreeVector(0., -1., 0.));
  REQUIRE(intersect.x() == Approx(0.));
  REQUIRE(intersect.y() == Approx(-minRad));
  REQUIRE(intersect.z() == Approx(0.));

}

TEST_CASE("Cylinder Arbitrary valid intersect") {
  // This test checks that for a sampler of arbitrary
  // position and arbitrary rotation a valid intersect
  // point is found for a randomly generated ray.
  auto minRad   = 200;
  auto maxRad   = 220;
  auto halfLen  = 500;
  auto origin   = G4ThreeVector(maxRad  * G4UniformRand(),
				maxRad  * G4UniformRand(),
				halfLen * G4UniformRand());
  auto rotation = new G4RotationMatrix();
  rotation->rotateX(CLHEP::twopi * G4UniformRand());
  rotation->rotateY(CLHEP::twopi * G4UniformRand());
  rotation->rotateZ(CLHEP::twopi * G4UniformRand());
  auto sampler = nexus::CylinderPointSampler2020(minRad, maxRad, halfLen,
						 0., CLHEP::twopi,
						 rotation, origin);
  auto point   = G4ThreeVector(minRad  * (2 * G4UniformRand() - 1),
			       minRad  * (2 * G4UniformRand() - 1),
			       halfLen * (2 * G4UniformRand() - 1));
  // Rotate and translate so inside Sampler Volume.
  point       *= *rotation;
  point       += origin;
  auto dir     = G4ThreeVector(G4UniformRand(),
			       G4UniformRand(),
			       G4UniformRand()).unit();

  auto intersect = sampler.GetIntersect(point, dir);
  
  // Check that the new ray passes through the original point.
  auto origin_point = point - intersect;
  REQUIRE(std::abs(origin_point.dot(dir)) == Approx(origin_point.mag()));
  
}
