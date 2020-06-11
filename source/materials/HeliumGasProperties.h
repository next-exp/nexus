// ----------------------------------------------------------------------------
// nexus | HeliumGasProperties.h
//
// This class collects the relevant physical properties of gaseous helium.
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

  class HeliumGasProperties
  {
  public:
    /// Constructor
    HeliumGasProperties(G4double pressure, G4double /*temperature=300*CLHEP::kelvin*/);
    /// Destructor
    ~HeliumGasProperties();

    static G4double Density(G4double pressure=15.*CLHEP::bar);
    static G4double MassPerMole(G4int mass_num=4);

    /// Electroluminescence yield of pure xenon gas
    G4double ELLightYield(G4double field_strength) const;


  private:
    G4double pressure_;
    //    G4double _temperature;

  };

} // end namespace nexus

#endif
