// ----------------------------------------------------------------------------
// nexus | XenonLiquidProperties.cc
//
// This class collects the relevant physical properties of liquid xenon.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef XENON_LIQUID_PROPERTIES_H
#define XENON_LIQUID_PROPERTIES_H

#include <globals.hh>
#include <vector>

class G4MaterialPropertiesTable;


namespace nexus {

  class XenonLiquidProperties
  {
  public:
    /// Constructor
    XenonLiquidProperties();
    /// Destructor
    ~XenonLiquidProperties();

    G4double Density();

    /// Return the refractive index of xenon gas for a given photon energy
    G4double RefractiveIndex(G4double energy);

    G4double Scintillation(G4double energy);
    void Scintillation(G4int entries, G4double* energy, G4double* intensity);


  private:
    G4double density_;

    //static const G4double _densities[100];

  };

} // end namespace nexus

#endif
