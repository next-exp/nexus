#include <Interpolation.h>

#include <catch.hpp>
using Catch::Matchers::Contains;


TEST_CASE("Linear Interpolation") {
  // These tests check thet Interpolation::LinearInterpolation
  // gives the correct result

  SECTION ("Small numbers"){
    Approx target = Approx(.0015).epsilon(0.01);
    G4double y = nexus::LinearInterpolation(.0015, .0010, .0020, .0010, .0020);
    REQUIRE (y == target);
  }

  SECTION ("Large numbers"){
    Approx target = Approx(15000).epsilon(0.01);
    G4double y = nexus::LinearInterpolation(15000, 10000, 20000, 10000, 20000);
    REQUIRE (y == target);
  }

  SECTION ("Negative numbers"){
    Approx target = Approx(-1.5).epsilon(0.01);
    G4double y = nexus::LinearInterpolation(-1.5, -2.0, -1.0, -2.0, -1.0);
    REQUIRE (y == target);

    target = Approx(0.0).epsilon(0.01);
    y = nexus::LinearInterpolation(0.0, -1.0, 1.0, -1.0, 1.0);
    REQUIRE (y == target);
  }

  SECTION ("Input zeros"){
    Approx target = Approx(0.5).epsilon(0.1);
    G4double y = nexus::LinearInterpolation(0.5, 0.0, 1.0, 0.0, 1.0);
    REQUIRE (y == target);
  }

  SECTION ("Edge cases"){
    G4double y = nexus::LinearInterpolation(1.0, 1.0, 2.0, 3.0, 4.0);
    REQUIRE (y == 3.0);

    y = nexus::LinearInterpolation(2.0, 1.0, 2.0, 3.0, 4.0);
    REQUIRE (y == 4.0);

    REQUIRE_THROWS (nexus::LinearInterpolation(1.0, 1.0, 1.0, 1.0, 2.0), Contains("x interval not valid"));

    y = nexus::LinearInterpolation(1.5, 1.0, 2.0, 2.0, 2.0);
    REQUIRE (y == 2.0);
  }

  SECTION ("x not in range"){
    REQUIRE_THROWS (nexus::LinearInterpolation(5.0, 1.0, 2.0, 3.0, 4.0),
                    Contains("x is not in the given interval"));
  }

}

TEST_CASE("Bilinear Interpolation"){
  // These tests check thet Interpolation::BilinearInterpolation
  // gives the correct result

  SECTION ("Small numbers"){
    Approx target = Approx(.002).epsilon(0.01);
    G4double y = nexus::BilinearInterpolation(.0015, .0010, .0020,
                                              .0015, .0010, .0020,
                                              .0010, .002, .002, .003);
    REQUIRE (y == target);
  }

  SECTION ("Large numbers"){
    Approx target = Approx(20000).epsilon(0.01);
    G4double y = nexus::BilinearInterpolation(15000, 10000, 20000,
                                              15000, 10000, 20000,
                                              10000, 20000, 20000, 30000);
    REQUIRE (y == target);
  }

  SECTION ("Negative numbers"){
    Approx target = Approx(-2.0).epsilon(0.01);
    G4double y = nexus::BilinearInterpolation(-1.5, -2.0, -1.0,
                                              -1.5, -2.0, -1.0,
                                              -1.0, -2.0, -2.0, -3.0);
    REQUIRE (y == target);

    target = Approx(0.0).epsilon(0.01);
    y = nexus::BilinearInterpolation(0.0, -1.0, 1.0,
                                     0.0, -1.0, 1.0,
                                     -1.0, 1.0, -1.0, 1.0);
    REQUIRE (y == target);
  }

  SECTION ("Input zeros"){
    Approx target = Approx(0.1).epsilon(0.1);
    G4double y = nexus::BilinearInterpolation(0.5, 0.0, 1.0,
                                              0.5, 0.0, 1.0,
                                              0.0, 0.1, 0.1, 0.2);
    REQUIRE (y == target);
  }

  SECTION ("Edge cases"){
    G4double y = nexus::BilinearInterpolation(1.0, 1.0, 2.0,
                                              1.0, 1.0, 2.0,
                                              1.0, 2.0, 3.0, 4.0);
    REQUIRE (y == 1.0);

    y = nexus::BilinearInterpolation(1.0, 1.0, 2.0,
                                     2.0, 1.0, 2.0,
                                     1.0, 2.0, 3.0, 4.0);
    REQUIRE (y == 2.0);

    y = nexus::BilinearInterpolation(2.0, 1.0, 2.0,
                                     1.0, 1.0, 2.0,
                                     1.0, 2.0, 3.0, 4.0);
    REQUIRE (y == 3.0);

    y = nexus::BilinearInterpolation(2.0, 1.0, 2.0,
                                     2.0, 1.0, 2.0,
                                     1.0, 2.0, 3.0, 4.0);
    REQUIRE (y == 4.0);

    REQUIRE_THROWS (nexus::BilinearInterpolation(1.0, 1.0, 1.0,
                                                 1.5, 1.0, 2.0,
                                                1.0, 2.0, 3.0, 4.0),
                    "One or more interval is invalid");

    REQUIRE_THROWS (nexus::BilinearInterpolation(1.5, 1.0, 2.0,
                                                 1.0, 1.0, 1.0,
                                                 1.0, 2.0, 3.0, 4.0),
                    "One or more interval is invalid");

    REQUIRE_THROWS (nexus::BilinearInterpolation(1.0, 1.0, 1.0,
                                                 1.0, 1.0, 1.0,
                                                 1.0, 2.0, 3.0, 4.0),
                    "One or more interval is invalid");
  }

  SECTION ("x or y not in input range"){
    REQUIRE_THROWS (nexus::BilinearInterpolation(0.0, 1.0, 2.0,
                                                 1.5, 1.0, 2.0,
                                                 1.0, 2.0, 3.0, 4.0),
                    "x or y is not in the given interval");

    REQUIRE_THROWS (nexus::BilinearInterpolation(3.0, 1.0, 2.0,
                                                 1.5, 1.0, 2.0,
                                                 1.0, 2.0, 3.0, 4.0),
                    "x or y is not in the given interval");

    REQUIRE_THROWS (nexus::BilinearInterpolation(1.5, 1.0, 2.0,
                                                 0.0, 1.0, 2.0,
                                                 1.0, 2.0, 3.0, 4.0),
                    "x or y is not in the given interval");

    REQUIRE_THROWS (nexus::BilinearInterpolation(1.5, 1.0, 2.0,
                                                 3.0, 1.0, 2.0,
                                                 1.0, 2.0, 3.0, 4.0),
                    "x or y is not in the given interval");
  }
}
