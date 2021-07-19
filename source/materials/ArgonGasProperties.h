// ----------------------------------------------------------------------------
// nexus | ArgonGasProperties.h
//
// Relevant physical properties of gaseous argon.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------


#ifndef ARGON_GAS_PROPERTIES_H
#define ARGON_GAS_PROPERTIES_H

#include <globals.hh>
#include <vector>
#include <CLHEP/Units/SystemOfUnits.h>

class G4MaterialPropertiesTable;


namespace nexus {

  G4double ArgonDensity(G4double pressure=5.*CLHEP::bar);

    /// Electroluminescence yield of pure argon gas
  G4double ArgonELLightYield(G4double field_strength, G4double pressure);

} // end namespace nexus

#endif
