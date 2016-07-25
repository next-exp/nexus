
///
///  Copyright (c) 2010-2016 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __ARGON_GAS_PROPERTIES__
#define __ARGON_GAS_PROPERTIES__

#include <globals.hh>
#include <vector>
#include <CLHEP/Units/SystemOfUnits.h>

class G4MaterialPropertiesTable;


namespace nexus {

  class ArgonGasProperties
  {
  public:
    /// Constructor
    ArgonGasProperties(G4double pressure=5.*CLHEP::bar, G4double temperature=300*CLHEP::kelvin);
    /// Destructor
    ~ArgonGasProperties();

    /// Return the refractive index of xenon gas for a given photon energy
    //   G4double RefractiveIndex(G4double energy);
    
    /* G4double Scintillation(G4double energy); */
    /* void Scintillation(G4int entries, G4double* energy, G4double* intensity); */

    static G4double Density(G4double pressure=5.*CLHEP::bar, G4double temperature=300*CLHEP::kelvin);
    

    /// Electroluminescence yield of pure xenon gas
    G4double ELLightYield(G4double field_strength) const;
    

  private:
    G4double _pressure;
    //    G4double _temperature;
    
  };

} // end namespace nexus

#endif
