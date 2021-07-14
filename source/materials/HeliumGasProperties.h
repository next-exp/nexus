// ----------------------------------------------------------------------------
// nexus | HeliumGasProperties.h
//
// Relevant physical properties of gaseous helium.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef HELIUM_GAS_PROPERTIES_H
#define HELIUM_GAS_PROPERTIES_H

#include <globals.hh>
#include <vector>
#include <CLHEP/Units/SystemOfUnits.h>

class G4MaterialPropertiesTable;


namespace nexus {


  G4double HeliumDensity(G4double pressure=15.*CLHEP::bar);
  G4double HeliumMassPerMole(G4int mass_num=4);

  /// Electroluminescence yield of pure helium gas
  G4double ELLightYield(G4double field_strength);

} // end namespace nexus

#endif
