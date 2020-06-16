#include "XenonGasProperties.h"

#include <G4SystemOfUnits.hh>

#include <setjmp.h>

#include <csignal>

#include <catch.hpp>

int val;
jmp_buf env_buffer;

void handle_sigabrt(int sig){
  // Handles SIGABRT being raised, by restoring environment
  // saved in env_buffer and returns sig
  longjmp(env_buffer, sig);
}

TEST_CASE("XenonGasProperties::Density") {
  // These tests check that the XenonGasProperties class correctly finds
  // the density of xenon given a temperature and pressure.

  auto props = nexus::XenonGasProperties();

  SECTION ("Density Calculation"){
    Approx target = Approx(87.836).epsilon(0.01);
    G4double density = props.GetDensity(15 * bar, 295 * kelvin);
    REQUIRE (density/(kg/m3) == target);
  }

  //  go to handle_sigabrt when SIGABRT is raised (raised by G4Exceptions)
  signal(SIGABRT, handle_sigabrt);

  SECTION ("Pressure is too big") {
    val = setjmp(env_buffer);
    if (val != 0){
      goto test1;
    }
    props.GetDensity(51 * bar, 295 * kelvin);
    test1:
      REQUIRE(val == SIGABRT);
  }

  SECTION ("Pressure is too small") {
    val = setjmp(env_buffer);
    if (val != 0){
      goto test2;
    }
    props.GetDensity(-15 * bar, 295 * kelvin);
    test2:
      REQUIRE(val == SIGABRT);
  }

  SECTION ("Temperature is too big"){
    val = setjmp(env_buffer);
    if (val != 0){
      goto test3;
    }
    props.GetDensity(15 * bar, 410 * kelvin);
    test3:
      REQUIRE(val == SIGABRT);
  }

  SECTION ("Temperature is too small"){
    val = setjmp(env_buffer);
    if (val != 0){
      goto test4;
    }
    props.GetDensity(15 * bar, -295 * kelvin);
    test4:
      REQUIRE(val == SIGABRT);
  }

}
