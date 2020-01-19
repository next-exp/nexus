
///
///  Copyright (c) 2010-2016 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __HELIUM_GAS_PROPERTIES__
#define __HELIUM_GAS_PROPERTIES__

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
    G4double _pressure;
    //    G4double _temperature;

  };

} // end namespace nexus

#endif
