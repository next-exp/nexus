#include "XenonProperties.h"

#include <G4SystemOfUnits.hh>

#include <catch.hpp>


TEST_CASE("XenonProperties::MakeDataTable"){
  auto props = nexus::XenonProperties();
  REQUIRE_NOTHROW(props.MakeDataTable());
}

TEST_CASE("XenonProperties::Density") {
  // These tests check that the XenonProperties class correctly finds
  // the density of xenon given a temperature and pressure.

  auto props = nexus::XenonProperties();

  SECTION ("Density Calculation"){
    Approx target = Approx(87.836).epsilon(0.01);
    G4double density = props.GetGasDensity(15 * bar, 295 * kelvin);
    REQUIRE (density/(kg/m3) == target);
  }

  SECTION ("Pressure is too big") {
    REQUIRE_THROWS (props.GetGasDensity(51 * bar, 295 * kelvin),
                    "Unknown xenon density for this pressure");
  }

  SECTION ("Pressure is too small") {
    REQUIRE_THROWS (props.GetGasDensity(-15 * bar, 295 * kelvin),
                    "Unknown xenon density for this pressure");
  }

  SECTION ("Temperature is too big"){
    REQUIRE_THROWS (props.GetGasDensity(15 * bar, 410 * kelvin),
                    "Unknown xenon density for this temperature");
  }

  SECTION ("Temperature is too small"){
    REQUIRE_THROWS (props.GetGasDensity(15 * bar, -295 * kelvin),
                    "Unknown xenon density for this temperature");
  }

}
